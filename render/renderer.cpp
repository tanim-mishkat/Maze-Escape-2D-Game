#include "renderer.h"
#include "colors.h"
#include "../utils/coords.h"
#include <GL/freeglut.h>
#include <cmath>

namespace Renderer
{
    namespace
    {
        float maxFloat(float a, float b)
        {
            return a > b ? a : b;
        }
    }

    void drawFilledRect(float x, float y, float width, float height,
                        float r, float g, float b, float a)
    {
        glColor4f(r, g, b, a);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }

    void drawRectOutline(float x, float y, float width, float height,
                         float lineWidth, float r, float g, float b, float a)
    {
        glLineWidth(lineWidth);
        glColor4f(r, g, b, a);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }

    void drawFloor(const BoardMetrics &metrics)
    {
        for (int row = 0; row < metrics.rows; row++)
        {
            for (int col = 0; col < metrics.cols; col++)
            {
                ScreenPos pos = Coords::gridToScreen(metrics, row, col);
                float shade = ((row + col) % 2 == 0) ? Config::FLOOR_SHADE_DARK : Config::FLOOR_SHADE_LIGHT;
                drawFilledRect(pos.x, pos.y, metrics.tileSize, metrics.tileSize,
                               shade, shade + Config::FLOOR_SHADE_GRADIENT, shade + Config::FLOOR_SHADE_GRADIENT * 2);
            }
        }
    }

    void drawMaze(const Maze &maze)
    {
        const BoardMetrics &metrics = maze.getMetrics();
        for (int row = 0; row < maze.getRows(); row++)
        {
            for (int col = 0; col < maze.getCols(); col++)
            {
                if (maze.getTile(row, col) == TILE_WALL)
                {
                    ScreenPos pos = Coords::gridToScreen(metrics, row, col);
                    drawFilledRect(pos.x, pos.y, metrics.tileSize, metrics.tileSize,
                                   Colors::WALL_DARK_R, Colors::WALL_DARK_G, Colors::WALL_DARK_B);
                    drawFilledRect(pos.x + metrics.tileInnerPadding,
                                   pos.y + metrics.tileInnerPadding,
                                   metrics.tileInnerSize,
                                   metrics.tileInnerSize,
                                   Colors::WALL_LIGHT_R, Colors::WALL_LIGHT_G, Colors::WALL_LIGHT_B);
                    drawRectOutline(pos.x + metrics.tileInnerPadding,
                                    pos.y + metrics.tileInnerPadding,
                                    metrics.tileInnerSize,
                                    metrics.tileInnerSize,
                                    maxFloat(1.0f, metrics.tileSize * 0.03f),
                                    0.72f, 0.86f, 1.0f, 0.6f);
                }
            }
        }
    }

    void drawExit(const Maze &maze, float animationTime)
    {
        const BoardMetrics &metrics = maze.getMetrics();
        GridPos exitPos = maze.getExitPos();
        ScreenPos pos = Coords::gridToScreen(metrics, exitPos);
        float pulse = Config::EXIT_PULSE_AMPLITUDE + Config::EXIT_PULSE_AMPLITUDE * std::sin(animationTime * Config::EXIT_PULSE_FREQUENCY);
        float glowInset = metrics.tileSize * Config::EXIT_GLOW_INSET_RATIO;
        float poleX = pos.x + metrics.tileSize * Config::EXIT_OUTER_RING_RATIO;
        float poleY = pos.y + metrics.tileSize * Config::EXIT_INNER_RING_RATIO;
        float poleWidth = maxFloat(2.0f, metrics.tileSize * 0.08f);
        float poleHeight = metrics.tileSize * 0.60f;
        float flagWidth = metrics.tileSize * (Config::EXIT_OUTER_RING_RATIO + pulse * 0.12f);
        float flagOffsetY = metrics.tileSize * Config::EXIT_INNER_RING_RATIO;

        drawFilledRect(pos.x, pos.y, metrics.tileSize, metrics.tileSize,
                       0.18f, 0.26f, 0.12f, 0.45f);

        drawFilledRect(pos.x + glowInset * 0.25f,
                       pos.y + glowInset,
                       metrics.tileSize - glowInset * 0.5f,
                       metrics.tileSize - glowInset * 2.0f,
                       1.0f, 0.88f, 0.35f, 0.14f + pulse * 0.12f);

        drawFilledRect(poleX, poleY, poleWidth, poleHeight, 0.92f, 0.95f, 1.0f);
        drawFilledRect(poleX - metrics.tileSize * 0.08f,
                       poleY - metrics.tileSize * 0.04f,
                       metrics.tileSize * 0.20f,
                       maxFloat(2.0f, metrics.tileSize * 0.06f),
                       0.40f, 0.28f, 0.12f);

        glColor4f(0.2f, 0.96f - pulse * 0.10f, 0.3f, 1.0f);

        glBegin(GL_TRIANGLES);
        glVertex2f(poleX + poleWidth,
                   poleY + poleHeight - flagOffsetY * 0.25f);
        glVertex2f(poleX + poleWidth + flagWidth,
                   poleY + poleHeight - flagOffsetY * 0.60f);
        glVertex2f(poleX + poleWidth,
                   poleY + poleHeight - flagOffsetY);
        glEnd();

        drawRectOutline(pos.x + metrics.tileInnerPadding,
                        pos.y + metrics.tileInnerPadding,
                        metrics.tileInnerSize,
                        metrics.tileInnerSize,
                        maxFloat(1.0f, metrics.tileSize * 0.03f),
                        1.0f, 0.94f, 0.60f, 0.55f);
    }

    void drawPlayer(const Player &player, const BoardMetrics &metrics)
    {
        ScreenPos pos = Coords::gridToScreen(metrics, player.getGridPos());
        float x = pos.x + metrics.playerPadding;
        float y = pos.y + metrics.playerPadding;
        float size = metrics.playerSize;
        float shadowOffset = maxFloat(1.0f, metrics.tileSize * 0.06f);
        float innerInset = maxFloat(1.5f, size * 0.16f);

        drawFilledRect(x + shadowOffset, y - shadowOffset, size, size, 0.0f, 0.0f, 0.0f, 0.22f);
        drawFilledRect(x, y, size, size, Colors::PLAYER_R, Colors::PLAYER_G, Colors::PLAYER_B);
        drawFilledRect(x + innerInset, y + innerInset,
                       size - innerInset * 2.0f, size - innerInset * 2.0f,
                       Colors::PLAYER_LIGHT_R, Colors::PLAYER_LIGHT_G, Colors::PLAYER_LIGHT_B);
        drawRectOutline(x, y, size, size,
                        maxFloat(1.0f, metrics.tileSize * 0.05f),
                        0.95f, 1.0f, 0.97f, 0.9f);
    }

    float getGameCanvasWidth(const BoardMetrics &metrics)
    {
        return metrics.boardWidth + metrics.sidePanelWidth + 52.0f;
    }

    float getGameCanvasHeight(const BoardMetrics &metrics)
    {
        return metrics.boardHeight + 40.0f;
    }

    float getGameCanvasX(int windowWidth, const BoardMetrics &metrics)
    {
        float x = (windowWidth - getGameCanvasWidth(metrics)) * 0.5f;
        return x < 16.0f ? 16.0f : x;
    }

    float getGameCanvasY(int windowHeight, const BoardMetrics &metrics)
    {
        float y = (windowHeight - getGameCanvasHeight(metrics)) * 0.5f;
        return y < 16.0f ? 16.0f : y;
    }

    float getBoardOriginX(int windowWidth, const BoardMetrics &metrics)
    {
        return getGameCanvasX(windowWidth, metrics) + metrics.canvasMargin;
    }

    float getBoardOriginY(int windowHeight, const BoardMetrics &metrics)
    {
        return getGameCanvasY(windowHeight, metrics) + metrics.canvasMargin;
    }

    float getSidebarX(int windowWidth, const BoardMetrics &metrics)
    {
        return getBoardOriginX(windowWidth, metrics) + metrics.boardWidth + metrics.sidebarMargin;
    }

    float getSidebarY(int windowHeight, const BoardMetrics &metrics)
    {
        return getBoardOriginY(windowHeight, metrics);
    }
}
