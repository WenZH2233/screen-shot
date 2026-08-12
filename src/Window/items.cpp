#include "Window/Items/button.h"
#include "Window/Items/font.h"

#include "log.h"

#include <core/SkFontMgr.h>
#include <ports/SkFontMgr_empty.h>

#include <algorithm>
#include <vector>

using namespace core;
using namespace window;

Button::Button (const Rect& rect) : rect(rect) {}
void Button::draw(SkCanvas* canvas) {
    if(!visible) return;
    SkPaint paint;
    paint.setColor(isHovered ? SK_ColorLTGRAY : SK_ColorGRAY);
    canvas->drawRect(SkRect::MakeLTRB(rect.getX(), rect.getY(), rect.getEX(), rect.getEY()), paint);
}

TextButton::TextButton(const std::string& text, core::Font* font, const core::Color& textColor) : Button(), font(font), textColor(textColor) {
    this->text = text;
}

SvgButton::SvgButton(const core::Rect& rect, const std::string& svgCode) : Button(rect) {
    setSvgCode(svgCode);
}

void SvgButton::draw(SkCanvas* canvas) {
    if(!visible) return;
    if(!svgDom) return;
    svgDom->setContainerSize(SkSize::Make(rect.getWidth(), rect.getHeight()));

    canvas->save();
    canvas->translate(rect.getX(), rect.getY());
    svgDom->render(canvas);
    canvas->restore();
}


bool core::Font::loadFromFile(const std::string& path, SkScalar size) {
    auto fontMgr = SkFontMgr_New_Custom_Empty();
    auto typeface = fontMgr ? fontMgr->makeFromFile(path.c_str()) : nullptr;
    if (!typeface) {
        Log.level(Level::ERROR) << "Failed to load font from file: " << path;
        return false;
    }
    skFont = std::make_unique<SkFont>();
    skFont->setTypeface(typeface);
    skFont->setSize(size);
    return true;
}

void Font::drawText(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint) {
    canvas->drawSimpleText(text.c_str(), text.size(), SkTextEncoding::kUTF8, point.getX(), point.getY(), *skFont.get(), paint);
}

void Font::drawTextBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint) {
    if (text.empty() || rect.getWidth() <= 0 || rect.getHeight() <= 0) {
        return;
    }

    const textMetrics sampleMetrics = measureText("A");
    const float lineHeight = std::max(sampleMetrics.height, 1.0f) * 1.2f;
    const float maxWidth = rect.getWidth();
    const float maxHeight = rect.getHeight();
    const int maxLines = std::max(1, static_cast<int>(maxHeight / lineHeight));

    std::vector<std::string> lines;
    std::string currentLine;

    for (size_t i = 0; i < text.size(); ++i) {
        const char ch = text[i];
        if (ch == '\n') {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
            }
            if (lines.size() >= static_cast<size_t>(maxLines)) {
                break;
            }
            continue;
        }

        const std::string candidate = currentLine + ch;
        if (measureText(candidate).width <= maxWidth) {
            currentLine = candidate;
            continue;
        }

        if (!currentLine.empty()) {
            lines.push_back(currentLine);
            if (lines.size() >= static_cast<size_t>(maxLines)) {
                break;
            }
            currentLine.clear();
        }

        const std::string singleChar(1, ch);
        if (measureText(singleChar).width > maxWidth) {
            currentLine.clear();
            break;
        }

        currentLine = singleChar;
    }

    if (!currentLine.empty() || lines.empty()) {
        lines.push_back(currentLine);
    }

    if (lines.size() > static_cast<size_t>(maxLines)) {
        lines.resize(maxLines);
    }

    if (!lines.empty() && lines.size() == static_cast<size_t>(maxLines) && !text.empty()) {
        std::string lastLine = lines.back();
        while (!lastLine.empty() && measureText(lastLine + "...").width > maxWidth) {
            lastLine.pop_back();
        }
        if (lastLine.empty()) {
            lines.back() = "...";
        } else {
            lines.back() = lastLine + "...";
        }
    }

    const float totalHeight = lines.size() * lineHeight;
    const float startY = rect.getY() + std::max(0.0f, (rect.getHeight() - totalHeight) / 2.0f) + lineHeight * 0.8f;

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        const float lineWidth = measureText(line).width;
        const float x = rect.getX() + (rect.getWidth() - lineWidth) / 2;
        const float y = startY + i * lineHeight;
        drawText(canvas, line, core::Point(x, y), paint);
    }
}

void Font::drawTextVertical(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint) {
    float x = point.getX();
    float y = point.getY();
    for(char c : text) {
        std::string s(1, c);
        textMetrics metrics = measureText(s);
        drawText(canvas, s, core::Point(x, y), paint);
        y += metrics.height;
    }
}

void Font::drawTextVerticalBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint) {
    if (text.empty() || rect.getWidth() <= 0 || rect.getHeight() <= 0) {
        return;
    }

    const textMetrics sampleMetrics = measureText("A");
    const float maxWidth = rect.getWidth();
    const float maxHeight = rect.getHeight();
    const float lineHeight = std::max(sampleMetrics.height, 1.0f) * 1.2f;
    const int maxLines = std::max(1, static_cast<int>(maxHeight / lineHeight));

    std::vector<std::string> lines;
    std::string currentLine;

    for (size_t i = 0; i < text.size(); ++i) {
        const char ch = text[i];
        if (ch == '\n') {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
            }
            if (lines.size() >= static_cast<size_t>(maxLines)) {
                break;
            }
            continue;
        }

        const std::string candidate = currentLine + ch;
        if (measureText(candidate).width <= maxWidth) {
            currentLine = candidate;
            continue;
        }

        if (!currentLine.empty()) {
            lines.push_back(currentLine);
            if (lines.size() >= static_cast<size_t>(maxLines)) {
                break;
            }
            currentLine.clear();
        }

        const std::string singleChar(1, ch);
        if (measureText(singleChar).width > maxWidth) {
            currentLine.clear();
            break;
        }

        currentLine = singleChar;
    }

    if (!currentLine.empty() || lines.empty()) {
        lines.push_back(currentLine);
    }

    if (lines.size() > static_cast<size_t>(maxLines)) {
        lines.resize(maxLines);
    }

    if (!lines.empty() && lines.size() == static_cast<size_t>(maxLines) && !text.empty()) {
        std::string lastLine = lines.back();
        while (!lastLine.empty() && measureText(lastLine + "...").width > maxWidth) {
            lastLine.pop_back();
        }
        if (lastLine.empty()) {
            lines.back() = "...";
        } else {
            lines.back() = lastLine + "...";
        }
    }

    const float totalHeight = lines.size() * lineHeight;
    const float startY = rect.getY() + std::max(0.0f, (rect.getHeight() - totalHeight) / 2.0f) + lineHeight * 0.8f;

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        const float lineWidth = measureText(line).width;
        const float x = rect.getX() + (rect.getWidth() - lineWidth) / 2;
        const float y = startY + i * lineHeight;
        drawText(canvas, line, core::Point(x, y), paint);
    }
}


textMetrics Font::measureText(const std::string& text) {
    SkRect bounds;
    skFont->measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);
    return {bounds.width(), bounds.height()};
}

textMetrics Font::measureTextVertical(const std::string& text) {
    float width = 0;
    float height = 0;
    for(char c : text) {
        std::string s(1, c);
        textMetrics metrics = measureText(s);
        width = std::max(width, metrics.width);
        height += metrics.height;
    }
    return {width, height};
}