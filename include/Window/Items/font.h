#pragma once

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
        Font(const SkFont& font) : skFont(std::make_unique<SkFont>(font)) {}
        Font(sk_sp<SkTypeface> typeface, SkScalar size) : skFont(std::make_unique<SkFont>(typeface, size)) {}
        Font(sk_sp<SkTypeface> typeface) : skFont(std::make_unique<SkFont>(typeface)) {}
        Font(sk_sp<SkTypeface> typeface, SkScalar size, SkScalar scaleX, SkScalar skewX) : skFont(std::make_unique<SkFont>(typeface, size, scaleX, skewX)) {}
        bool loadFromFile(const std::string& path, SkScalar size);
        operator const SkFont&() const {
            return *skFont.get();
        }
        void drawText(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint);
        void drawTextBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint);
        void drawTextVertical(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint);
        void drawTextVerticalBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint);
        textMetrics measureText(const std::string& text);
        textMetrics measureTextVertical(const std::string& text);
};
}