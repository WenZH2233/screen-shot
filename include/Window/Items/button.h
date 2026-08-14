#pragma once

#include <functional>
#include <string>
#include <core/SkCanvas.h>
#include <core/SkStream.h>
#include <core/SkBitmap.h>

#include "font.h"
#include "rect.h"
#include "color.h"

struct resvg_render_tree;

namespace window {
    class Button {
    public:
        Button(const core::Rect& rect);
        Button(){}
        virtual ~Button() = default;
        
        void onDraw(SkCanvas* canvas);
        virtual void draw(SkCanvas* canvas);

        bool click(float x, float y){if(enabled && rect.contains(x, y) && onClick) { onClick(); return true; } return false;}
        bool MouseMove(float x, float y){if(enabled && rect.contains(x, y))isHovered=true; else isHovered=false; return isHovered;}
        void setOnClick(std::function<void()> newOnClick){ this->onClick = newOnClick; }
        void setVisible(bool visible){this->visible = visible;setEnabled(visible);}
        void setEnabled(bool enabled){this->enabled = enabled;}
        void setBackgroundColor(const core::Color& color){this->backgroundColor = color;}
        void setBackgroundHoverColor(const core::Color& color){this->backgroundHoverColor = color;}
        void setBorderColor(const core::Color& color){this->borderColor = color;}
        void setRect(const core::Rect& rect){this->rect = rect;}
        void setText(const std::string& text){this->text = text;}
        void setBorderRadius(float radius){this->borderRadius = radius;}
        void setBorderWidth(float width){this->borderWidth = width;}
        void resize(int windowWidth, int windowHeight){this->rect.resize(windowWidth, windowHeight);}
    protected:
        std::function<void()> onClick;
        bool visible=true;
        bool enabled=true;
        bool isHovered=false;
        std::string text;
        core::Color backgroundColor=SK_ColorTRANSPARENT; 
        core::Color backgroundHoverColor=0x80000000;
        core::Color borderColor=SK_ColorBLACK;
        core::Rect rect{0,0,0,0};
        float borderRadius=0;
        float borderWidth=0;
    };
    class TextButton : public Button {
    public:
        TextButton(const std::string& text, core::Font* font, const core::Color& textColor = SK_ColorBLACK);
        TextButton(){}
        void setFont(core::Font* font){this->font = font;}
        void setTextColor(const core::Color& color){this->textColor = color;}
        void setShowText(bool showText){this->showText = showText;}
    protected:
        core::Font* font;
        core::Color textColor;
        bool showText=true;
    };
}