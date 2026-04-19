#ifndef TEXT_H
#define TEXT_H

namespace TextRenderer
{
    int getTextWidth(const char* text);
    void drawText(float x, float y, const char* text);
    void drawTextWithShadow(float x, float y, const char* text, float r, float g, float b);
    void clearWidthCache();  // call on reshape if font ever changes
}

#endif // TEXT_H
