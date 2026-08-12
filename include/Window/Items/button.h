#pragma once

#include <functional>
#include <string>
#include <core/SkCanvas.h>
#include <modules/svg/include/SkSVGDOM.h>
#include <core/SkStream.h>
#include "font.h"
#include "rect.h"
#include "color.h"

namespace window {
    class Button {
    public:
        Button(const core::Rect& rect);
        Button(){}
        void draw(SkCanvas* canvas);
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
    public:
        SvgButton(const core::Rect& rect, const std::string& svgCode);
        SvgButton(){}
        void setSvgCode(const std::string& code){
            auto stream = SkMemoryStream::MakeCopy(code.data(), code.size());
            svgDom.reset();
            svgDom = SkSVGDOM::MakeFromStream(*stream);
        }
        void draw(SkCanvas* canvas);
    protected:
        sk_sp<SkSVGDOM> svgDom;
    };
}