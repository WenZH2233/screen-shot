#include "Window/Items/font.h"

#include "log.h"

#include <core/SkCanvas.h>
#include <core/SkFontMgr.h>
#include <core/SkFont.h>
#include <ports/SkFontMgr_empty.h>


using namespace core;


bool core::Font::loadFromFile(const std::string& path, SkScalar size) {
    auto fontMgr = SkFontMgr_New_Custom_Empty();
    auto typeface = fontMgr ? fontMgr->makeFromFile(path.c_str()) : nullptr;
    Log.level(Level::INFO) << "Loading font from file: " << path << " with size: " << size << op::endl;
    if (!typeface) {
        Log.level(Level::ERROR) << "Failed to load font from file: " << path << " with size: " << size << op::endl;
        return false;
    }
    skFont = std::make_unique<SkFont>();
    skFont->setTypeface(typeface);
    skFont->setSize(size);
    return true;
}

void Font::drawText(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint, float scale) {
    if(scale<=0.0f)return;
    SkFont scaledFont = *skFont.get();
    scaledFont.setSize(skFont->getSize() * scale);

    canvas->drawSimpleText(text.c_str(), text.size(), SkTextEncoding::kUTF8, point.getX(), point.getY(), scaledFont, paint);
}

void Font::drawTextBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint, float scale) {
    if (text.empty() || rect.getWidth() <= 0 || rect.getHeight() <= 0) {
        return;
    }

    const textMetrics sampleMetrics = measureText("A", scale);
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
        drawText(canvas, line, core::Point(x, y), paint, scale);
    }
}

void Font::drawTextFill(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint) {
    if(text.empty() || rect.getWidth() <= 0 || rect.getHeight() <= 0) {
        return;
    }
    textMetrics metrics = measureText(text);
    float scaleX = rect.getWidth() / metrics.width;
    float scaleY = rect.getHeight() / metrics.height;
    float scale = std::min(scaleX, scaleY);
    float scaledWidth = metrics.width * scale;
    float scaledHeight = metrics.height * scale;
    float x = rect.getX() + (rect.getWidth() - scaledWidth) / 2;
    float y = rect.getY() + (rect.getHeight() - scaledHeight) / 2 + scaledHeight * 0.8f;
    drawText(canvas, text, core::Point(x, y), paint, scale);
}

void Font::drawTextVertical(SkCanvas* canvas, const std::string& text, core::Point point, const SkPaint& paint, float scale) {
    float x = point.getX();
    float y = point.getY();
    for(char c : text) {
        std::string s(1, c);
        textMetrics metrics = measureText(s, scale);
        drawText(canvas, s, core::Point(x, y), paint, scale);
        y += metrics.height;
    }
}

void Font::drawTextVerticalBetween(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint, float scale) {
    if (text.empty() || rect.getWidth() <= 0 || rect.getHeight() <= 0) {
        return;
    }

    const textMetrics sampleMetrics = measureText("A", scale);
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
        if (measureText(candidate, scale).width <= maxWidth) {
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
        if (measureText(singleChar, scale).width > maxWidth) {
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
        while (!lastLine.empty() && measureText(lastLine + "...", scale).width > maxWidth) {
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
        const float lineWidth = measureText(line, scale).width;
        const float x = rect.getX() + (rect.getWidth() - lineWidth) / 2;
        const float y = startY + i * lineHeight;
        drawText(canvas, line, core::Point(x, y), paint, scale);
    }
}

void Font::drawTextVerticalFill(SkCanvas* canvas, const std::string& text, core::Rect rect, const SkPaint& paint) {
    if(text.empty() || rect.getWidth() <= 0 || rect.getHeight() <= 0) {
        return;
    }
    textMetrics metrics = measureTextVertical(text);
    float scaleX = rect.getWidth() / metrics.width;
    float scaleY = rect.getHeight() / metrics.height;
    float scale = std::min(scaleX, scaleY);
    float scaledWidth = metrics.width * scale;
    float scaledHeight = metrics.height * scale;
    float x = rect.getX() + (rect.getWidth() - scaledWidth) / 2;
    float y = rect.getY() + (rect.getHeight() - scaledHeight) / 2 + scaledHeight * 0.8f;
    float currentY = y;
    for(char c : text) {
        if(c=='\n') continue;
        std::string s(1, c);
        textMetrics charMetrics = measureText(s, 1.0f);
        float charWidth = charMetrics.width * scale;
        float charHeight = charMetrics.height * scale;
        float charX = x + (scaledWidth - charWidth) / 2;
        float charY = currentY+charHeight*0.8f;
        drawText(canvas, s, core::Point(charX, charY), paint, scale);
        currentY += charMetrics.height;
    }
}

textMetrics Font::measureText(const std::string& text, float scale) {
    SkRect bounds;
    skFont->measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);
    return {bounds.width() * scale, bounds.height() * scale};
}

textMetrics Font::measureTextVertical(const std::string& text, float scale) {
    float width = 0;
    float height = 0;
    for(char c : text) {
        std::string s(1, c);
        textMetrics metrics = measureText(s, scale);
        width = std::max(width, metrics.width);
        height += metrics.height;
    }
    return {width, height};
}