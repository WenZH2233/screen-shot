#pragma once
#include <string>
#include <unordered_map>


namespace core{
class ConfigItem{
    std::string value;
public:
    ConfigItem(const std::string& value);
    ConfigItem();
    const std::string& getOriginalValue() const;

    std::string toString() const;
    std::string toPath() const;
    int toInt() const;
    double toDouble() const;
    bool toBool() const;

    operator std::string() const;
    operator int() const;
    operator double() const;
    operator bool() const;
};
class Config{
    static Config* instance;
    static Config* defaultInstance;
    static std::string ConfigFilePath;
    bool isDefault = false;
    void readConfigFile();
    void setDefaultConfig();
    Config(bool isDefault=false);
    std::unordered_map<std::string, ConfigItem> configItems;
public:
    static Config& getInstance(bool isDefault = false);
    void init(const std::string& configFilePath);
    ConfigItem& get(const std::string& key);
    void set(const std::string& key, const ConfigItem& value);
    bool setifno(const std::string& key, const ConfigItem& value);
    void saveConfigFile();
};
}

#include "configKeys.h"