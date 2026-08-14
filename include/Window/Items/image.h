#pragma once

#include "rect.h"
#include <core/SkCanvas.h>
#include <resvg.h>

namespace window {

    class Image{
    protected:
        int width=0;
        int height=0;   
        core::Rect rect{};
    public:
        Image(){}
        Image(const core::Rect& rect) : rect(rect), width(rect.getWidth()), height(rect.getHeight()) {}
        void setRect(const core::Rect& rect){this->rect = rect; this->width = rect.getWidth(); this->height = rect.getHeight();resize();}
        void setSize(int width, int height){this->width = width; this->height = height;resize();}
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        const core::Rect& getRect() const { return rect; }

        virtual void draw(SkCanvas* canvas) = 0;
        virtual void resize(int windowWidth, int windowHeight)=0;
    };

    class SvgImage : public Image{
        resvg_render_tree* tree = nullptr;
        sk_sp<SkImage> skImage=nullptr;
        const char* svgCode=nullptr;
        bool needRebuildTree=true;
        void praseSvg();
    public:
        SvgImage(const core::Rect& rect, const char* svgCode) : Image(rect), svgCode(svgCode) {praseSvg();}
        SvgImage(){}
        ~SvgImage();
        void draw(SkCanvas* canvas) override;
        void resize(int windowWidth, int windowHeight) override;
        void setSvgCode(const char* svgCode);
    };
}