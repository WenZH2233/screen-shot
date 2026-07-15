#pragma once

#include <string>

extern bool g_Quited;

namespace core {
    void init();
    void exit();
    extern std::string appPath,dataPath;
}