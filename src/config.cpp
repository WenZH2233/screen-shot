#include "config.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "log.h"
#include "version.h"

namespace core {

    Config* Config::instance = nullptr;
    Config* Config::defaultInstance = nullptr;
    std::string Config::ConfigFilePath;
    Config::Config(bool isDefalut) :isDefault(isDefalut) {}

    Config& Config::getInstance(bool isDefault) {
        if (isDefault) {
            if (!defaultInstance) {
                defaultInstance = new Config();
            }
            return *defaultInstance;
        }
        else {
            if (!instance) {
                instance = new Config();
            }
            return *instance;
        }
    }
    void Config::init(const std::string& configFilePath) {
        ConfigFilePath = configFilePath;
        readConfigFile();
        setDefaultConfig();
    }

    void Config::readConfigFile()
    {
        std::ifstream ConfigFile(ConfigFilePath);
        if (!ConfigFile.is_open())
        {
            Log.level(Level::ERROR) << "Failed to open config file: " << ConfigFilePath << op::endl;
            return;
        }
        nlohmann::json ConfigJson;
        try
        {
            ConfigJson = nlohmann::json::parse(ConfigFile);
        }
        catch (std::exception e)
        {
            Log.level(Level::ERROR) << "Failed to parse config file: " << ConfigFilePath << "Error info: " << e.what() << op::endl;
        }
        catch (...)
        {
            Log.level(Level::ERROR) << "Failed to parse config file: " << ConfigFilePath << "Error info: Unknown error" << op::endl;
        }
        std::string configVersion = ConfigJson.value("version", "0");
		if (configVersion != VERSION_STRING) {
            Log.level(Level::INFO) << "Config file version mismatch. Expected: " << VERSION_STRING << ", Found: " << configVersion << "updating config file" << op::endl;
			ConfigJson["version"] = VERSION_STRING;
		}
        try
        {
            nlohmann::json configItemsJson = ConfigJson.value("configItems", nlohmann::json::object());
            for (const auto& configItemJson : configItemsJson)
            {
                configItems[configItemJson.value("name", "")] = ConfigItem(configItemJson.value("value", ""));
            }
        }
        catch (...)
        {
            Log.level(Level::ERROR) << "Failed to read config items from config file: " << ConfigFilePath << op::endl;
        }
    }
	ConfigItem& Config::get(const std::string& key) {
		if (configItems.find(key) == configItems.end()) {
			if (isDefault) {
				Log.level(Level::ERROR) << "Default config item not found: " << key << op::endl;
				return configItems[""];
			}
			Log.level(Level::WARNING) << "Config item not found: " << key << op::endl;
			return Config::getInstance(true).get(key);
		}
		return configItems[key];
	}
	void Config::set(const std::string& key, const ConfigItem& value) {
		configItems[key] = value;
	}
	void Config::setifno(const std::string& key, const ConfigItem& value) {
		if(!isDefault)Config::getInstance(true).set(key, value);
        if (configItems.find(key) == configItems.end()) {
			set(key, value);
		}
	}
    void Config::saveConfigFile() {
		nlohmann::json ConfigJson;
		ConfigJson["version"] = VERSION_STRING;
		ConfigJson["fullVersion"] = VERSION_FULL_STRING;
		nlohmann::json configItemsJson;
		for (const auto& [key, value] : configItems) {
			nlohmann::json configItemJson;
			configItemJson["name"] = key;
			configItemJson["value"] = value.getOriginalValue();
			configItemsJson.push_back(configItemJson);
		}
		ConfigJson["configItems"] = configItemsJson;
    }
}
