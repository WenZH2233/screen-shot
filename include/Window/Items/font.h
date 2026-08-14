#pragma once

#include <core/SkCanvas.h>
#include <core/SkFont.h>
#include <rect.h>

namespace core{
    struct textMetrics {
        float width;
        float height;
    };
    class Font{
        std::unique_ptr<SkFont> skFont;
    public:
        Font() : skFont(std::make_unique<SkFont>()) {}
        Font(const SkFont& font) : skFont(std::make_unique<SkFont>(font)) {}
        Font(const std::string& path, SkScalar size) : skFont(std::make_unique<SkFont>()) { loadFromFile(path, size); }
        Font(sk_sp<SkTypeface> typeface, SkScalar size) : skFont(std::make_unique<SkFont>(typeface, size)) {}
        Font(sk_sp<SkTypeface> typeface) : skFont(std::make_unique<SkFont>(typeface)) {}
        Font(sk_sp<SkTypeface> typeface, SkScalar size, SkScalar scaleX, SkScalar skewX) : skFont(std::make_unique<SkFont>(typeface, size, scaleX, skewX)) {}
        bool loadFromFile(const std::string& path, SkScalar size);
        operator const SkFont&() const {
            return *skFont.get();
        }
        int getSize() const { return skFont->getSize(); }
        void setSize(SkScalar size) { skFont->setSize(size); }
        void drawText(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint, float scale=1.0f);
        void drawTextBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint, float scale=1.0f);
        void drawTextFill(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint);
        void drawTextVertical(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint, float scale=1.0f);
        void drawTextVerticalBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint, float scale=1.0f);
        void drawTextVerticalFill(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint);
        textMetrics measureText(const std::string& text, float scale=1.0f);
        textMetrics measureTextVertical(const std::string& text, float scale=1.0f);
};
}