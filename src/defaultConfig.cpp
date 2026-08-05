#include "config.h"

#include "hotkey.h"

static const std::unordered_map<std::string, std::string> defaultConfigItems = {
    {HOTKRY_SHOT_SCREEN, ""},
    {LANG, "zh_CN"},
    
};

namespace core
{
    void Config::setDefaultConfig()
    {
        bool missingKeys = true;
        for(const auto& [key, defaultValue]:defaultConfigItems){
            missingKeys |= setifno(key, ConfigItem(defaultValue));
        }
        if (missingKeys) {
            saveConfigFile();
        }
    }
}