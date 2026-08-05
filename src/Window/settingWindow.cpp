#include "Window/settingWindow.h"

using namespace window;


settingWindow::settingWindow(int w, int h, const std::string& title) :
    Window(WindowType::SettingWindow, w, h, title) {}

void settingWindow::onDraw(SkCanvas* canvas) {
}
