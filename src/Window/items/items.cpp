#include "Window/Items/button.h"
#include "Window/Items/image.h"

#include "log.h"

#include <core/SkImage.h>
#include <resvg.h>
#include <algorithm>
#include <vector>

using namespace core;
using namespace window;

Button::Button (const Rect& rect) : rect(rect) {}

void Button::onDraw(SkCanvas* canvas) {
    if(!visible) return;
    SkPaint paint;
    paint.setColor(isHovered ? backgroundHoverColor : backgroundColor);
    paint.setAntiAlias(true);
    if(borderRadius > 0) {
        canvas->drawRoundRect(rect, borderRadius, borderRadius, paint);
    } else {
        canvas->drawRect(rect, paint);
    }
    draw(canvas);
    if(borderWidth > 0) {
        SkPaint borderPaint;
        borderPaint.setColor(borderColor);
        borderPaint.setStyle(SkPaint::kStroke_Style);
        borderPaint.setStrokeWidth(borderWidth);
        borderPaint.setAntiAlias(true);
        float halfBorderWidth = borderWidth / 2.0f;
        Rect borderRect = rect;
        borderRect.setX(borderRect.getOX() + halfBorderWidth);
        borderRect.setY(borderRect.getOY() + halfBorderWidth);
        borderRect.setWidth(borderRect.getWidth() - 2 * halfBorderWidth);
        borderRect.setHeight(borderRect.getHeight() - 2 * halfBorderWidth);
        if(borderRadius > 0) {
            canvas->drawRoundRect(borderRect, borderRadius, borderRadius, borderPaint);
        } else {
            canvas->drawRect(borderRect, borderPaint);
        }
    }
}

void Button::draw(SkCanvas* canvas) {
}

TextButton::TextButton(const std::string& text, core::Font* font, const core::Color& textColor) : Button(), font(font), textColor(textColor) {
    this->text = text;
}

SvgImage::~SvgImage() {
    if (tree) {
        resvg_tree_destroy(tree);
        tree = nullptr;
    }
}

void SvgImage::praseSvg() {
    if (tree) {
        resvg_tree_destroy(tree);
        tree = nullptr;
    }
    if(!svgCode) {
        Log.level(Level::ERROR) << "SVG code is null"<< op::endl;
        return;
    }
    if(width <= 0 || height <= 0) {
        Log.level(Level::WARNING) << "Invalid width or height for SVG image"<< op::endl;
        width=rect.getWidth();
        height=rect.getHeight();
        if(width <= 0 || height <= 0) {
            Log.level(Level::ERROR) << "Width and height are still invalid after using rect dimensions"<< op::endl;
            return;
        }
    }
    if(needRebuildTree) {
        needRebuildTree = false;
        resvg_options* options = resvg_options_create();
        if(!options) {
            Log.level(Level::ERROR) << "Failed to create resvg options"<< op::endl;
            return;
        }
        int err=resvg_parse_tree_from_data(svgCode, strlen(svgCode), options, &tree);
        resvg_options_destroy(options);
        if(err!=0 || !tree) {
            Log.level(Level::ERROR) << "Failed to parse SVG code"<< op::endl;
            tree = nullptr;
            return;
        }
    }
    resvg_size size = resvg_get_image_size(tree);
    float scaleX = static_cast<float>(width) / size.width;
    float scaleY = static_cast<float>(height) / size.height;
    float scale = std::min(scaleX, scaleY);
    float newWidth = size.width * scale;
    float newHeight = size.height * scale;
    float offsetX = (width - newWidth) / 2.0f;
    float offsetY = (height - newHeight) / 2.0f;

    resvg_transform transform;
    transform.a = scale;
    transform.b = 0;
    transform.c = 0;
    transform.d = scale;
    transform.e = offsetX;
    transform.f = offsetY;

    SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    SkBitmap bitmap;
    if (!bitmap.tryAllocPixels(info)) {
        Log.level(Level::ERROR) << "Failed to allocate pixels for SVG image"<< op::endl;
        return;
    }
    resvg_render(tree, transform, width, height, reinterpret_cast<char*>(bitmap.getPixels()));
    skImage = SkImages::RasterFromBitmap(bitmap);
}

void SvgImage::draw(SkCanvas* canvas) {
    if(!skImage) {
        Log.level(Level::ERROR) << "SVG image is not ready to draw"<< op::endl;
        return;
    }
    canvas->drawImage(skImage, rect.getX(), rect.getY());
}

void SvgImage::resize() {
    praseSvg();
}

void SvgImage::setSvgCode(const char* svgCode) {
    this->svgCode = svgCode;
    needRebuildTree = true;
    praseSvg();
}