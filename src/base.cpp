#include "base.h"

#include "log.h"
#include "config.h"
#include "lang.h"
#include "color.h"
#include "Window/Window.h"
#include "native_theme_bridge.h"
#include <tray.hpp>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <windows.h>
#include <ShlObj_core.h>
#else
#include <unistd.h>
#endif

#include <unicode/unistr.h>
#include <unicode/utypes.h>

#undef ERROR

std::atomic_bool g_Quited = false;
static std::thread trayThread;

namespace core {
    std::string appPath,dataPath,systemLanguage;
    std::atomic_bool isDarkMode = true;
    Color systemAccentColor;
    Color systemBackgroundColor;
    Color systemTextColor;
    
    
    void init() {
        std::setlocale(LC_CTYPE, ".UTF-8");
        core_init::getAppPath();
        core_init::getDataPath();
        core_init::getSystemLanguage();
        Log.init();
		refreshSystemThemeFromRust();
        if (!glfwInit()) {
            Log.level(Level::ERROR) << "Failed to initialize GLFW" << op::endl;
            return;
        }
        Log.level(Level::INFO) << "appPath: " << appPath <<op::endl;
        Log.level(Level::INFO) << "dataPath: " << dataPath << op::endl;
        Log.level(Level::INFO) << "systemLanguage: " << systemLanguage << op::endl;
        Log.level(Level::INFO) << "systemAccentColor: " << systemAccentColor.toHexStr() << op::endl;
        Log.level(Level::INFO) << "systemBackgroundColor: " << systemBackgroundColor.toHexStr() << op::endl;
        Log.level(Level::INFO) << "systemTextColor: " << systemTextColor.toHexStr() << op::endl;
        Log.level(Level::INFO) << "isDarkMode: " << isDarkMode << op::endl;
		Config::getInstance().init(dataPath + "/config.json");
        Lang::getInstance();
		trayThread = std::thread(core_init::initTray);
		trayThread.detach();
    }

    void exit() {
		if (trayThread.joinable())trayThread.join();
        glfwTerminate();
		g_Quited = true;
		Log.exit();
    }

    std::string wstring2string(const std::wstring& wstr){
        icu::UnicodeString ustr(wstr.c_str());
        std::string str;
        ustr.toUTF8String(str);
        return str;
    }
    std::wstring string2wstring(const std::string& str){
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(str);
        std::wstring wstr;
        wstr.resize(ustr.length());
        ustr.extract(0, ustr.length(), &wstr[0]);
        return wstr;
    }
    std::string utf82gbk(const std::string& utf8_str){
#ifdef _WIN32
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, NULL, 0);
        std::wstring wstr(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], len);

        len = WideCharToMultiByte(936, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        std::string gbk_str(len, '\0');
        WideCharToMultiByte(936, 0, wstr.c_str(), -1, &gbk_str[0], len, NULL, NULL);

        return gbk_str;
#else
        // 在非 Windows 平台上，直接返回原始字符串
        return utf8_str;
#endif
    }

    uint32_t Color::toHex() {
        return (static_cast<uint32_t>(a) << 24) |
         (static_cast<uint32_t>(r) << 16) |
          (static_cast<uint32_t>(g) << 8) |
           static_cast<uint32_t>(b);
    }
    std::string Color::toHexStr() {
        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%02X%02X%02X%02X", r, g, b, a);
        return std::string(buffer);
    }


    namespace core_init{

        void initTray(){
            using namespace Tray;
#ifdef _WIN32
            // 设置托盘图标为应用程序图标
            std::string iconPath = appPath + "/files/img/icon.ico";
#else
			std::string iconPath = appPath + "/files/img/icon.png";
#endif
            Log.level(Level::INFO) << "Tray icon path: " << iconPath << op::endl;
			::Tray::Tray tray("screen-shot", iconPath);
			tray.addEntry(Button(utf82gbk("tray.exit"_lang), [&]() {
				tray.exit();
                g_Quited = true;
                for(auto& [type, windowPtr] : window::windows) {
                    if (windowPtr) {
                        windowPtr->stop();
                    }
                }
			}));
            tray.addEntry(Button(utf82gbk("tray.settings"_lang), [&]() {
                window::windows[window::WindowType::SettingWindow]->show();
            }));
			tray.run();
        }

        void getAppPath(){
#ifdef _WIN32
            //用Windows API获取当前程序的路径
            char buffer[MAX_PATH];
            GetModuleFileNameA(NULL, buffer, MAX_PATH);
            std::string path(buffer);
            path = path.substr(0, path.find_last_of("\\/"));
            appPath = path;
#else
            //用Linux API获取当前程序的路径
            char buffer[1024];
            ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
            if (len != -1) {
                buffer[len] = '\0';
                std::string path(buffer);
                path = path.substr(0, path.find_last_of("\\/"));
                appPath = path;
            }
#endif
            if(appPath.back()=='\\'||appPath.back()=='/') appPath.pop_back();
        }
        void getDataPath(){
#ifdef _WIN32
            //获取APPDATA路径
            char buffer[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer);
            std::string path(buffer);
            dataPath=path+"/screen-shot/";
#else
            const char* home = std::getenv("HOME");
            dataPath=std::string(home)+"/.screen-shot/";
#endif
            if(dataPath.back()=='\\'||dataPath.back()=='/') dataPath.pop_back();
        }
        void getSystemLanguage(){
            std::string lang;
#ifdef _WIN32
            // Windows: 使用 GetUserDefaultLocaleName
            wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
            if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH)) {
                // 将宽字符转换为窄字符串 (UTF-8)
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, localeName, -1, nullptr, 0, nullptr, nullptr);
                if (size_needed > 0) {
                    std::string utf8_str(size_needed, 0);
                    WideCharToMultiByte(CP_UTF8, 0, localeName, -1, &utf8_str[0], size_needed, nullptr, nullptr);
                    lang = utf8_str;
                    // 移除末尾的空字符
                    if (!lang.empty() && lang.back() == '\0') {
                        lang.pop_back();
                    }
                }
            }
            // 失败时的备用方案
            if (lang.empty()) {
                lang = "en-US"; // 或其他默认值
            }
#else
            // POSIX (Linux, macOS): 检查环境变量
            const char* env_vars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
            for (const char* var : env_vars) {
                const char* val = std::getenv(var);
                if (val && val[0] != '\0') {
                    lang = val;
                    // 提取语言部分 (例如 "zh_CN.UTF-8" -> "zh_CN")
                    size_t dot_pos = lang.find('.');
                    if (dot_pos != std::string::npos) {
                        lang = lang.substr(0, dot_pos);
                    }
                    break;
                }
            }
            // 失败时的备用方案
            if (lang.empty()) {
                lang = "en_US";
            }
#endif
            systemLanguage=lang;
        }
    }
}