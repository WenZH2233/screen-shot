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
        virtual void draw(SkCanvas* canvas);
        bool click(float x, float y){if(enabled && rect.contains(x, y) && onClick) { onClick(); return true; } return false;}
        bool MouseMove(float x, float y){if(enabled && rect.contains(x, y))isHovered=true; else isHovered=false; return isHovered;}
        void setOnClick(std::function<void()> newOnClick){ this->onClick = newOnClick; }
        void setVisible(bool visible){this->visible = visible;setEnabled(visible);}
        void setEnabled(bool enabled){this->enabled = enabled;}
        void setBackgroundColor(const core::Color& color){this->backgroundColor = color;}
        void setBackgroundHoverColor(const core::Color& color){this->backgroundHoverColor = color;}
        void setRect(const core::Rect& rect){this->rect = rect;}
        void setText(const std::string& text){this->text = text;}
    protected:
        std::function<void()> onClick;
        bool visible=true;
        bool enabled=true;
        bool isHovered=false;
        std::string text;
        core::Color backgroundColor=SK_ColorGRAY;
        core::Color backgroundHoverColor=SK_ColorLTGRAY;
        core::Rect rect{0,0,0,0};
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
    class SvgButton : public Button {
        bool parseSvg(const std::string& svgCode);
        resvg_render_tree* tree = nullptr;
        std::string svgCode;
        float imageWidth = 0;
        float imageHeight = 0;
    public:
        SvgButton(const core::Rect& rect, const std::string& svgCode);
        SvgButton(){}
        ~SvgButton() override;
        void loadSvg(const std::string& svgCode, core::Color backgroundColor = SK_ColorTRANSPARENT);
        void setBackgroundColor(const core::Color& color){this->backgroundColor = color;}
        void setSvgCode(const std::string& svgCode){loadSvg(svgCode, backgroundColor);}
        void draw(SkCanvas* canvas)override;
    };
}