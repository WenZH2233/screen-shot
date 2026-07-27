#include "main.h"

int main(int argc, char* argv[]) {
	using namespace core;
    std::cout << "Hello, World!" << std::endl;
    core::init();

    std::string langStr = Lang::langType2String(Lang::getInstance()->getCurrentLangType());

    Log.level(Level::INFO) << "Current Language: " << langStr << op::endl;
	Log.level(Level::INFO) << "Translation for 'hello': " << "menu.pin"_lang << op::endl;

    core::exit();
}