#include "text.h"
#include <GL/freeglut.h>

namespace TextRenderer
{
    int getTextWidth(const char* text)
    {
        int width = 0;
        while (*text != '\0')
        {
            width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *text);
            text++;
        }
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
