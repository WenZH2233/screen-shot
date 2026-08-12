#include "Window/settingWindow.h"

#include "rect.h"
#include "log.h"

using namespace window;
using namespace core;

settingWindow::settingWindow(int w, int h, const std::string& title) :
    Window(WindowType::SettingWindow, w, h, title) {}

void settingWindow::onDraw(SkCanvas* canvas) {
    SkPaint paint;
    paint.setColor(SK_ColorLTGRAY);
    SkPoint points[2] = {{0, 50}, {static_cast<SkScalar>(width), 50}};
    canvas->drawLine(points[0], points[1], paint);
    
}
