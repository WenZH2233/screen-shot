#include "base.h"

#include "log.h"
#include "config.h"
#include "lang.h"
#ifdef _WIN32
#include <windows.h>
#include <ShlObj_core.h>
#else
#include <unistd.h>
#endif

#include <unicode/unistr.h>
#include <unicode/utypes.h>


bool g_Quited = false;


namespace core {
    std::string appPath,dataPath,systemLanguage;
    void init() {
        std::setlocale(LC_CTYPE, ".UTF-8");
        core_init::getAppPath();
        core_init::getDataPath();
        core_init::getSystemLanguage();
        Log.init();
        Log.level(Level::INFO) << "appPath: " << appPath << " dataPath: " << dataPath << " systemLanguage: " << systemLanguage << op::endl;
		Config::getInstance().init(dataPath + "/config.json");
        Lang::getInstance();
    }

    void exit() {
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



    namespace core_init{
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