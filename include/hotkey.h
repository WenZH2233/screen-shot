#pragma once
#include <vector>
#include <string>

namespace core
{
    class hotkey
    {
        std::vector<int> keyCodes;
    public:
        hotkey() = default;
        hotkey(const std::vector<int>& keyCodes) : keyCodes(keyCodes) {}
        operator std::string();
        std::string toJsonString();
        std::string toString();
        bool ifPressed(std::vector<int> pressedKeyCodes);
        bool ifValid();
        void fromString(const std::string& value);
        static int fromStringToKeyCode(const std::string& value);
		static std::vector<int> fromStringToKeyCodes(const std::string& value);
        static const std::string& fromKeyCodeToString(int keyCode);
		static const std::string fromKeyCodeToString(std::vector<int> keyCodes);
    };

    std::vector<int> operator"" HK(const char* str, size_t len);
    const std::string& operator"" Key(unsigned long long int keyCode);
}
