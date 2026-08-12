#pragma once

#include "Window.h"
#include "Items/button.h"

#include "config.h"


namespace window{
    class settingItem{
        std::string name;
        core::ConfigItem* value;
        int number;
        core::Rect rect;
        window::TextButton* button;
    public:
        settingItem(const std::string& name, core::ConfigItem* value):name(name), value(value){}
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