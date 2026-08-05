#include "main.h"

int main(int argc, char* argv[]) {
	using namespace core;
    std::cout << "Hello, World!" << std::endl;
    core::init();


	Log.level(Level::INFO) << "systemLanguage: " << systemLanguage << op::endl;

	using namespace window;
	settingWindow settingWin(800, 600, "Setting Window");
	settingWin.start();
	while (!g_Quited) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	window::windows[window::WindowType::SettingWindow]->stopAndJoin();
    core::exit();
}