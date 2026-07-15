#include "base.h"

#include "log.h"
#include "config.h"
#ifdef _WIN32
#include <windows.h>
#include <ShlObj_core.h>
#else
#include <unistd.h>
#endif


bool g_Quited = false;


namespace core {
    void init() {
#ifdef _WIN32
        {
            //用Windows API获取当前程序的路径
            char buffer[MAX_PATH];
            GetModuleFileNameA(NULL, buffer, MAX_PATH);
            std::string path(buffer);
            appPath = path;
        }
        {
            //获取APPDATA路径
            char buffer[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer);
            std::string path(buffer);
            dataPath=path+"/screen-shot/";
        }

#else
        //用Linux API获取当前程序的路径
        char buffer[1024];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
            std::string path(buffer);
            appPath = path;
        }
        const char* home = std::getenv("HOME");
        dataPath=std::string(home)+"/.screen-shot/"
#endif
        Log.init();
        Log.level(Level::INFO) << "appPath: " << appPath << " dataPath: " << dataPath << op::endl;
		Config::getInstance().init(dataPath + "config.json");
    }

    void exit() {
		g_Quited = true;
		Log.exit();
    }
    std::string appPath,dataPath;
}