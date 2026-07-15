#include <hotkey.h>

#include "log.h"

#include <nlohmann/json.hpp>
#include <GLFW/glfw3.h>

extern const std::unordered_map<std::string, int> glfwKeyMap;

namespace core {
	hotkey::operator std::string() {
		return toString();
	}
	std::string hotkey::toJsonString() {
		nlohmann::json j = keyCodes;
		return j.dump();
	}
	std::string hotkey::toString() {
		return hotkey::fromKeyCodeToString(keyCodes);
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
	int hotkey::fromStringToKeyCode(const std::string& value) {
		auto it = glfwKeyMap.find(value);
		if (it != glfwKeyMap.end()) {
			return it->second;
		}
		Log.level(Level::WARNING) << "Unknown key string: " << value << op::endl;
		return GLFW_KEY_UNKNOWN;
	}
	std::vector<int> hotkey::fromStringToKeyCodes(const std::string& value) {
		std::vector<int> keyCodes;
		size_t start = 0;
		size_t end = value.find('+');
		while (end != std::string::npos) {
			std::string keyStr = value.substr(start, end - start);
			int keyCode = fromStringToKeyCode(keyStr);
			if (keyCode != GLFW_KEY_UNKNOWN) {
				keyCodes.push_back(keyCode);
			}
			start = end + 1;
			end = value.find('+', start);
		}
		// Handle the last key (or the only key if there are no '+')
		std::string lastKeyStr = value.substr(start);
		int lastKeyCode = fromStringToKeyCode(lastKeyStr);
		if (lastKeyCode != GLFW_KEY_UNKNOWN) {
			keyCodes.push_back(lastKeyCode);
		}
		return keyCodes;
	}
    const std::string& hotkey::fromKeyCodeToString(int keyCode) {
        static std::unordered_map<int, std::string> reserveMap;
		for (const auto& pair : glfwKeyMap) {
			if (reserveMap.find(pair.second) == reserveMap.end())
			    reserveMap[pair.second] = pair.first;
		}
		auto it = reserveMap.find(keyCode);
		if (it != reserveMap.end()) {
			return it->second;
		}
		Log.level(Level::WARNING) << "Unknown key code: " << keyCode << op::endl;
        static const std::string unknownKey = "unknown";
		return unknownKey;
    }
    const std::string hotkey::fromKeyCodeToString(std::vector<int> keyCodes) {
		std::string result;
		for (const auto& code : keyCodes) {
			result += fromKeyCodeToString(code) + "+";
		}
        if (result.size() > 0) {
			result.pop_back(); // Remove the trailing '+'
        }
		return result;
    }
	std::vector<int> operator"" HK(const char* str, size_t len) {
		return hotkey::fromStringToKeyCodes(std::string(str, len));
	}
	const std::string& operator"" Key(unsigned long long int keyCode) {
		return hotkey::fromKeyCodeToString(static_cast<int>(keyCode));
	}
}


static const std::unordered_map<std::string, int> glfwKeyMap = {
    // ========== 字母（A-Z） ==========
    {"a", GLFW_KEY_A},
    {"b", GLFW_KEY_B},
    {"c", GLFW_KEY_C},
    {"d", GLFW_KEY_D},
    {"e", GLFW_KEY_E},
    {"f", GLFW_KEY_F},
    {"g", GLFW_KEY_G},
    {"h", GLFW_KEY_H},
    {"i", GLFW_KEY_I},
    {"j", GLFW_KEY_J},
    {"k", GLFW_KEY_K},
    {"l", GLFW_KEY_L},
    {"m", GLFW_KEY_M},
    {"n", GLFW_KEY_N},
    {"o", GLFW_KEY_O},
    {"p", GLFW_KEY_P},
    {"q", GLFW_KEY_Q},
    {"r", GLFW_KEY_R},
    {"s", GLFW_KEY_S},
    {"t", GLFW_KEY_T},
    {"u", GLFW_KEY_U},
    {"v", GLFW_KEY_V},
    {"w", GLFW_KEY_W},
    {"x", GLFW_KEY_X},
    {"y", GLFW_KEY_Y},
    {"z", GLFW_KEY_Z},

    // ========== 数字（主键盘） ==========
    {"0", GLFW_KEY_0},
    {"1", GLFW_KEY_1},
    {"2", GLFW_KEY_2},
    {"3", GLFW_KEY_3},
    {"4", GLFW_KEY_4},
    {"5", GLFW_KEY_5},
    {"6", GLFW_KEY_6},
    {"7", GLFW_KEY_7},
    {"8", GLFW_KEY_8},
    {"9", GLFW_KEY_9},

    // ========== 功能键（F1-F25） ==========
    {"f1", GLFW_KEY_F1},
    {"f2", GLFW_KEY_F2},
    {"f3", GLFW_KEY_F3},
    {"f4", GLFW_KEY_F4},
    {"f5", GLFW_KEY_F5},
    {"f6", GLFW_KEY_F6},
    {"f7", GLFW_KEY_F7},
    {"f8", GLFW_KEY_F8},
    {"f9", GLFW_KEY_F9},
    {"f10", GLFW_KEY_F10},
    {"f11", GLFW_KEY_F11},
    {"f12", GLFW_KEY_F12},
    {"f13", GLFW_KEY_F13},
    {"f14", GLFW_KEY_F14},
    {"f15", GLFW_KEY_F15},
    {"f16", GLFW_KEY_F16},
    {"f17", GLFW_KEY_F17},
    {"f18", GLFW_KEY_F18},
    {"f19", GLFW_KEY_F19},
    {"f20", GLFW_KEY_F20},
    {"f21", GLFW_KEY_F21},
    {"f22", GLFW_KEY_F22},
    {"f23", GLFW_KEY_F23},
    {"f24", GLFW_KEY_F24},
    {"f25", GLFW_KEY_F25},

    // ========== 方向键 ==========
    {"up", GLFW_KEY_UP},
    {"down", GLFW_KEY_DOWN},
    {"left", GLFW_KEY_LEFT},
    {"right", GLFW_KEY_RIGHT},

    // ========== 修饰键（区分左右） ==========
    {"lshift", GLFW_KEY_LEFT_SHIFT},
    {"rshift", GLFW_KEY_RIGHT_SHIFT},

    {"lctrl", GLFW_KEY_LEFT_CONTROL},
    {"rctrl", GLFW_KEY_RIGHT_CONTROL},

    {"lalt", GLFW_KEY_LEFT_ALT},
    {"ralt", GLFW_KEY_RIGHT_ALT},

#ifdef _WIN32
    {"win", GLFW_KEY_LEFT_SUPER},     // 更常用的别名
#elif defined(_MACOS)
    {"command", GLFW_KEY_LEFT_SUPER},
#else
    {"super", GLFW_KEY_LEFT_SUPER},
#endif // _WIN32
    {"rsuper", GLFW_KEY_RIGHT_SUPER},

    // ========== 符号键（按美式键盘布局命名） ==========
    {"space", GLFW_KEY_SPACE},
    {"`", GLFW_KEY_GRAVE_ACCENT},
    {"-", GLFW_KEY_MINUS},
    {"=", GLFW_KEY_EQUAL},
    {"[", GLFW_KEY_LEFT_BRACKET},
    {"]", GLFW_KEY_RIGHT_BRACKET},
    {"\\", GLFW_KEY_BACKSLASH},
    {";", GLFW_KEY_SEMICOLON},
    {"'", GLFW_KEY_APOSTROPHE},
    {",", GLFW_KEY_COMMA},
    {".", GLFW_KEY_PERIOD},
    {"/", GLFW_KEY_SLASH},

    // ========== 控制键 ==========
    {"esc", GLFW_KEY_ESCAPE},
    {"escape", GLFW_KEY_ESCAPE},
    {"enter", GLFW_KEY_ENTER},
    {"return", GLFW_KEY_ENTER},
    {"tab", GLFW_KEY_TAB},
    {"backspace", GLFW_KEY_BACKSPACE},
    {"insert", GLFW_KEY_INSERT},
    {"del", GLFW_KEY_DELETE},
    {"delete", GLFW_KEY_DELETE},
    {"home", GLFW_KEY_HOME},
    {"end", GLFW_KEY_END},
    {"pageup", GLFW_KEY_PAGE_UP},
    {"pagedown", GLFW_KEY_PAGE_DOWN},
    {"pgup", GLFW_KEY_PAGE_UP},
    {"pgdn", GLFW_KEY_PAGE_DOWN},
    {"capslock", GLFW_KEY_CAPS_LOCK},
    {"scrolllock", GLFW_KEY_SCROLL_LOCK},
    {"numlock", GLFW_KEY_NUM_LOCK},
    {"printscreen", GLFW_KEY_PRINT_SCREEN},
    {"pause", GLFW_KEY_PAUSE},
    {"menu", GLFW_KEY_MENU},          // 右键菜单键

    // ========== 数字小键盘（带 KP_ 前缀） ==========
    {"kp0", GLFW_KEY_KP_0},
    {"kp1", GLFW_KEY_KP_1},
    {"kp2", GLFW_KEY_KP_2},
    {"kp3", GLFW_KEY_KP_3},
    {"kp4", GLFW_KEY_KP_4},
    {"kp5", GLFW_KEY_KP_5},
    {"kp6", GLFW_KEY_KP_6},
    {"kp7", GLFW_KEY_KP_7},
    {"kp8", GLFW_KEY_KP_8},
    {"kp9", GLFW_KEY_KP_9},
    {"kpdecimal", GLFW_KEY_KP_DECIMAL},
    {"kpdivide", GLFW_KEY_KP_DIVIDE},
    {"kpmultiply", GLFW_KEY_KP_MULTIPLY},
    {"kpsubtract", GLFW_KEY_KP_SUBTRACT},
    {"kpadd", GLFW_KEY_KP_ADD},
    {"kpenter", GLFW_KEY_KP_ENTER},
    {"kpequal", GLFW_KEY_KP_EQUAL},

    // ========== 其他 ==========
    {"unknown", GLFW_KEY_UNKNOWN},
};