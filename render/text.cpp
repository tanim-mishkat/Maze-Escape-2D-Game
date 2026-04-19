#include "text.h"
#include <GL/freeglut.h>
#include <string>
#include <unordered_map>

namespace TextRenderer
{
    static std::unordered_map<std::string, int> widthCache;

    void clearWidthCache()
    {
        widthCache.clear();
    }

    int getTextWidth(const char* text)
    {
        auto it = widthCache.find(text);
        if (it != widthCache.end())
            return it->second;

        int width = 0;
        for (const char* p = text; *p != '\0'; ++p)
            width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *p);

        widthCache[text] = width;
        return width;
    }

    void drawText(float x, float y, const char* text)
    {
        glRasterPos2f(x, y);
        while (*text != '\0')
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
            text++;
        }
    }

    void drawTextWithShadow(float x, float y, const char* text, float r, float g, float b)
    {
        glColor4f(0.0f, 0.0f, 0.0f, 0.70f);
        drawText(x + 2.0f, y - 2.0f, text);

        glColor3f(r, g, b);
        drawText(x, y, text);
    }
}
