#pragma once

#include "Window.h"
#include "Items/button.h"
#include "Items/image.h"

#include "config.h"


namespace window{
    class settingItem{
        std::string name;
        core::ConfigItem* value=nullptr;
        int number=0;
        core::Rect rect{};
        core::Point namePoint{};
        static std::unique_ptr<core::Font> font;
        std::unique_ptr<window::TextButton> button=nullptr;
        std::unique_ptr<window::SvgImage> icon=nullptr;
    public:
        settingItem(int number, const std::string& name, core::ConfigItem* value, int windowWidth, int windowHeight);
        void draw(SkCanvas* canvas);
    };
    class settingWindow: public Window{
        //class,page,item
        std::map<std::string, std::unique_ptr<std::vector<std::unique_ptr<settingItem>>>> settings;
    public:
        settingWindow(int w, int h, const std::string& title);
        void onDraw(SkCanvas* canvas) override;
    };
}