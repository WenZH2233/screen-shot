#pragma once

#include "Window.h"


namespace window{
    class settingWindow: public Window{
    public:
        settingWindow(int w, int h, const std::string& title);
        void onDraw(SkCanvas* canvas) override;
    };
}