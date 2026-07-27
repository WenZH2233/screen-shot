#pragma once

#include <string>

extern bool g_Quited;

namespace core {
    void init();
    void exit();
    extern std::string appPath,dataPath,systemLanguage;
    std::string wstring2string(const std::wstring& wstr);
    std::wstring string2wstring(const std::string& str);
    namespace core_init{
        void getAppPath();
        void getDataPath();
        void getSystemLanguage();
    }
}