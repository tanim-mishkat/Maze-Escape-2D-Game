#ifndef TEXT_H
#define TEXT_H

namespace TextRenderer
{
    int getTextWidth(const char* text);
    void drawText(float x, float y, const char* text);
    void drawTextWithShadow(float x, float y, const char* text, float r, float g, float b);
}

#endif // TEXT_H
