#include "config.h"

#include "hotkey.h"

namespace core
{
    void Config::setDefaultConfig()
    {
	    setifno(HOTKRY_SHOT_SCREEN, ConfigItem());
        setifno(LANG, ConfigItem("zh_CN"));
    }
}