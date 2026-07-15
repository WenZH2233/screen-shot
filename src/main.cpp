#include "main.h"

int main(int argc, char* argv[]) {
	using namespace core;
    std::cout << "Hello, World!" << std::endl;
    core::init();

    std::string hotkeyStr = "lshift";
	int keyCode = core::hotkey::fromStringToKeyCode(hotkeyStr);

	Log.level(Level::INFO) << "Hotkey string: " << hotkeyStr << ", Key code: " << keyCode << op::endl;
    keyCode = 340;
	Log.level(Level::INFO) << "Key code: " << keyCode << ", Hotkey string: " << core::hotkey::fromKeyCodeToString(keyCode) << op::endl;

	hotkeyStr = "lshift+f1";
	Log.level(Level::INFO) << "Hotkey string: " << hotkeyStr << ", Key codes: " << core::hotkey::fromStringToKeyCodes(hotkeyStr) << op::endl;
	std::vector<int> keyCodes = { GLFW_KEY_LEFT_ALT, GLFW_KEY_0 };
	Log.level(Level::INFO) << "Key codes: " << keyCodes << ", Hotkey string: " << core::hotkey::fromKeyCodeToString(keyCodes) << op::endl;

    core::exit();
}