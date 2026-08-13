#pragma once

#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include "color.h"

extern std::atomic_bool g_Quited;

namespace core {
    void init();
    void exit();
    extern std::string appPath,dataPath,systemLanguage;
    extern std::atomic_bool isDarkMode;
    extern Color systemAccentColor;
    extern Color systemBackgroundColor;
    extern Color systemTextColor;
    bool refreshSystemThemeFromRust();
    // resvg bridge (C++ 封装)
    std::wstring string2wstring(const std::string& str);
    std::string utf82gbk(const std::string& utf8_str);
    namespace core_init{
        void getAppPath();
        void getDataPath();
        void getSystemLanguage();
        void initTray();
        void initCheckIsDarkMode();
    }
}