#pragma once
#include <vector>
#include <string>

namespace core
{
	class hotkey
	{
		std::vector<int> keyCodes;
	public:
		operator std::string();
		std::string toString();
		bool ifPressed(std::vector<int> pressedKeyCodes);
		bool ifValid();
		void fromString(const std::string& value);
	};
}
