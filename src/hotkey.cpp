#include <hotkey.h>

#include "log.h"

#include <nlohmann/json.hpp>


namespace core {
	hotkey::operator std::string() {
		return toString();
	}
	std::string hotkey::toString() {
		nlohmann::json j = keyCodes;
		return j.dump();
	}
	bool hotkey::ifPressed(std::vector<int> pressedKeyCodes) {
		for (int code : keyCodes) {
			if (std::find(pressedKeyCodes.begin(), pressedKeyCodes.end(), code) == pressedKeyCodes.end()) {
				return false;
			}
		}
		return true;
	}
	bool hotkey::ifValid() {
		return !keyCodes.empty();
	}
	void hotkey::fromString(const std::string& value) {
		try {
			nlohmann::json j = nlohmann::json::parse(value);
			keyCodes = j.get<std::vector<int>>();
		}
		catch (const nlohmann::json::parse_error& e) {
			Log.level(Level::ERROR) << "Failed to parse hotkey from string: " << value << ". Error: " << e.what() << op::endl;
			keyCodes.clear();
		}
	}
}