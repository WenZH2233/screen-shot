#include "Window/settingWindow.h"

#include "rect.h"
#include "log.h"
#include "icons.h"
#include "base.h"

#include <memory>

using namespace window;
using namespace core;

std::unique_ptr<core::Font> settingItem::font = nullptr;

settingItem::settingItem(int number, const std::string& name,ConfigItem* value, int windowWidth, int windowHeight):number(number),name(name),value(value){
    if(!font){
        font = std::make_unique<core::Font>();
        std::string fontPath = core::appPath+"/files/fonts/text.ttf";
        font->loadFromFile(fontPath, 24);
    }
    float y=0.1f*number+0.05f;
    float yend=y+0.08f;
    icon = std::make_unique<window::SvgImage>();
    icon->resize(windowWidth, windowHeight);
    icon->setRect(core::Rect(0.05f, y, 0.1f, yend));
    icon->setSvgCode(Icons::All.at(name));
    rect = core::Rect(0.05f, y, 0.9f, yend);
    rect.resize(windowWidth, windowHeight);
    namePoint = core::Point(0.14f, y);
    namePoint.resize(windowWidth, windowHeight);
    button = std::make_unique<window::TextButton>(value->toString(), font.get(), core::Color(200,200,200));
    button->resize(windowWidth, windowHeight);
    button->setRect(core::Rect(0.7f, y, 0.95f, yend));
    button->setBorderColor(SK_ColorLTGRAY);
    button->setBackgroundColor(SK_ColorTRANSPARENT);
    button->setBackgroundHoverColor(core::Color(0,0,0,128));
}

void settingItem::draw(SkCanvas* canvas){
    SkPaint paint;
    paint.setColor(SK_ColorLTGRAY);
    font->drawText(canvas, name, namePoint, paint);
    icon->draw(canvas);
    button->onDraw(canvas);
}


settingWindow::settingWindow(int w, int h, const std::string& title) :
    Window(WindowType::SettingWindow, w, h, title) {
        this->settings["General"] = std::make_unique<std::vector<std::unique_ptr<settingItem>>>();
        this->settings["General"]->emplace_back(std::make_unique<settingItem>(0, LANG, &Config::getInstance().get(LANG)));
    }

void settingWindow::onDraw(SkCanvas* canvas) {
    SkPaint paint;
    paint.setColor(SK_ColorLTGRAY);
    SkPoint points[2] = {{0, 50}, {static_cast<SkScalar>(width), 50}};
    canvas->drawLine(points[0], points[1], paint);
    auto& items = settings["General"];
    for(auto& item: *items){
        item->draw(canvas);
    }
}
