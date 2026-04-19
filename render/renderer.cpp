#include "renderer.h"
#include "colors.h"
#include "../utils/coords.h"
#include <GL/freeglut.h>
#include <cmath>

namespace Renderer
{
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

    void drawFloor()
    {
        for (int row = 0; row < Config::MAZE_ROWS; row++)
        {
            for (int col = 0; col < Config::MAZE_COLS; col++)
            {
                ScreenPos pos = Coords::gridToScreen(row, col);
                float shade = ((row + col) % 2 == 0) ? 0.10f : 0.12f;
                drawFilledRect(pos.x, pos.y, Config::TILE_SIZE, Config::TILE_SIZE,
                             shade, shade + 0.02f, shade + 0.04f);
            }
        }
    }

    void drawMaze(const Maze& maze)
    {
        for (int row = 0; row < Config::MAZE_ROWS; row++)
        {
            for (int col = 0; col < Config::MAZE_COLS; col++)
            {
                if (maze.getTile(row, col) == TILE_WALL)
                {
                    ScreenPos pos = Coords::gridToScreen(row, col);
                    drawFilledRect(pos.x, pos.y, Config::TILE_SIZE, Config::TILE_SIZE,
                                 Colors::WALL_DARK_R, Colors::WALL_DARK_G, Colors::WALL_DARK_B);
                    drawFilledRect(pos.x + Config::TILE_INNER_PADDING, pos.y + Config::TILE_INNER_PADDING,
                                 Config::TILE_INNER_SIZE, Config::TILE_INNER_SIZE,
                                 Colors::WALL_LIGHT_R, Colors::WALL_LIGHT_G, Colors::WALL_LIGHT_B);
                    drawRectOutline(pos.x + Config::TILE_INNER_PADDING, pos.y + Config::TILE_INNER_PADDING,
                                  Config::TILE_INNER_SIZE, Config::TILE_INNER_SIZE,
                                  1.5f, 0.72f, 0.86f, 1.0f, 0.6f);
                }
            }
        }
    }

    void drawExit(const Maze& maze, float animationTime, bool hasKey)
    {
        GridPos exitPos = maze.getExitPos();
        ScreenPos pos = Coords::gridToScreen(exitPos);
        TileType exitTile = maze.getTile(exitPos);
        bool isLocked = (exitTile == TILE_LOCKED_EXIT);

        float pulse = 0.5f + 0.5f * std::sin(animationTime * 4.0f);
        float poleX = pos.x + 15.0f;
        float poleY = pos.y + 8.0f;
        float poleHeight = 30.0f;
        float flagWidth = 16.0f + pulse * 6.0f;
        float glowWidth = 18.0f + pulse * 6.0f;

        // Background with lock status color
        if (isLocked && !hasKey)
        {
            drawFilledRect(pos.x, pos.y, Config::TILE_SIZE, Config::TILE_SIZE,
                         0.26f, 0.18f, 0.18f, 0.45f);
        }
        else
        {
            drawFilledRect(pos.x, pos.y, Config::TILE_SIZE, Config::TILE_SIZE,
                         0.18f, 0.26f, 0.12f, 0.45f);
        }
        
        // Glow effect
        drawFilledRect(pos.x - 8.0f, pos.y + 8.0f, glowWidth,
                     Config::TILE_SIZE - 16.0f, 1.0f, 0.88f, 0.35f, 0.18f + pulse * 0.12f);

        // Pole
        drawFilledRect(poleX, poleY, 4.0f, poleHeight, 0.92f, 0.95f, 1.0f);
        drawFilledRect(poleX - 6.0f, poleY - 2.0f, 16.0f, 4.0f, 0.40f, 0.28f, 0.12f);

        // Flag color depends on lock status
        if (isLocked && !hasKey)
        {
            glColor4f(0.6f, 0.1f + pulse * 0.10f, 0.1f, 1.0f);
        }
        else
        {
            glColor4f(0.2f, 1.0f - pulse * 0.10f, 0.3f, 1.0f);
        }
        
        glBegin(GL_TRIANGLES);
            glVertex2f(poleX + 4.0f, poleY + poleHeight - 2.0f);
            glVertex2f(poleX + 4.0f + flagWidth, poleY + poleHeight - 7.0f);
            glVertex2f(poleX + 4.0f, poleY + poleHeight - 12.0f);
        glEnd();

        // Lock icon if locked
        if (isLocked && !hasKey)
        {
            float lockX = pos.x + Config::TILE_SIZE - 18.0f;
            float lockY = pos.y + 8.0f;
            drawFilledRect(lockX, lockY, 10.0f, 12.0f, 0.8f, 0.2f, 0.2f, 0.9f);
            drawFilledRect(lockX + 2.0f, lockY + 6.0f, 6.0f, 6.0f, 0.3f, 0.1f, 0.1f);
        }

        drawRectOutline(pos.x + Config::TILE_INNER_PADDING, pos.y + Config::TILE_INNER_PADDING,
                      Config::TILE_INNER_SIZE, Config::TILE_INNER_SIZE,
                      1.5f, 1.0f, 0.94f, 0.60f, 0.55f);
    }

    void drawTraps(const Maze& maze, float animationTime)
    {
        for (int row = 0; row < Config::MAZE_ROWS; row++)
        {
            for (int col = 0; col < Config::MAZE_COLS; col++)
            {
                if (maze.getTile(row, col) == TILE_TRAP)
                {
                    ScreenPos pos = Coords::gridToScreen(row, col);
                    float x = pos.x + 10.0f;
                    float y = pos.y + 10.0f;
                    float size = Config::TILE_SIZE - 20.0f;
                    float blink = 0.5f + 0.5f * std::sin(animationTime * 7.0f + row + col);

                    drawFilledRect(x, y, size, size,
                                 0.55f + blink * 0.35f, 0.10f + blink * 0.10f, 0.14f + blink * 0.12f);
                    drawRectOutline(x, y, size, size, 2.0f, 1.0f, 0.70f, 0.70f, 0.85f);

                    glLineWidth(3.0f);
                    glColor4f(1.0f, 0.90f, 0.90f, 0.85f);
                    glBegin(GL_LINES);
                        glVertex2f(x + 6.0f, y + 6.0f);
                        glVertex2f(x + size - 6.0f, y + size - 6.0f);
                        glVertex2f(x + size - 6.0f, y + 6.0f);
                        glVertex2f(x + 6.0f, y + size - 6.0f);
                    glEnd();
                }
            }
        }
    }

    void drawObstacles(const Maze& maze)
    {
        for (int row = 0; row < Config::MAZE_ROWS; row++)
        {
            for (int col = 0; col < Config::MAZE_COLS; col++)
            {
                if (maze.getTile(row, col) == TILE_OBSTACLE)
                {
                    ScreenPos pos = Coords::gridToScreen(row, col);
                    float x = pos.x + 8.0f;
                    float y = pos.y + 8.0f;
                    float size = Config::TILE_SIZE - 16.0f;

                    drawFilledRect(x, y, size, size, 0.28f, 0.22f, 0.18f);
                    drawFilledRect(x + 5.0f, y + 5.0f, size - 10.0f, size - 10.0f,
                                 0.50f, 0.40f, 0.30f);
                    drawRectOutline(x, y, size, size, 2.0f, 0.84f, 0.74f, 0.58f, 0.75f);
                }
            }
        }
    }

    void drawKeys(const Maze& maze, float animationTime)
    {
        for (int row = 0; row < Config::MAZE_ROWS; row++)
        {
            for (int col = 0; col < Config::MAZE_COLS; col++)
            {
                if (maze.getTile(row, col) == TILE_KEY)
                {
                    ScreenPos pos = Coords::gridToScreen(row, col);
                    float x = pos.x + Config::TILE_SIZE / 2.0f;
                    float y = pos.y + Config::TILE_SIZE / 2.0f;
                    float bounce = 3.0f * std::sin(animationTime * 5.0f);
                    float glow = 0.3f + 0.3f * std::sin(animationTime * 6.0f);

                    // Glow effect
                    drawFilledRect(pos.x + 8.0f, pos.y + 8.0f,
                                 Config::TILE_SIZE - 16.0f, Config::TILE_SIZE - 16.0f,
                                 1.0f, 0.84f, 0.0f, glow);

                    // Key body
                    float keyX = x - 8.0f;
                    float keyY = y + bounce - 6.0f;
                    drawFilledRect(keyX, keyY, 16.0f, 4.0f, 1.0f, 0.84f, 0.0f);
                    
                    // Key head (circle approximation)
                    drawFilledRect(keyX - 4.0f, keyY - 2.0f, 6.0f, 8.0f, 1.0f, 0.84f, 0.0f);
                    drawFilledRect(keyX - 2.0f, keyY - 4.0f, 2.0f, 12.0f, 1.0f, 0.84f, 0.0f);
                    
                    // Key teeth
                    drawFilledRect(keyX + 12.0f, keyY + 4.0f, 2.0f, 3.0f, 1.0f, 0.84f, 0.0f);
                    drawFilledRect(keyX + 15.0f, keyY + 4.0f, 2.0f, 2.0f, 1.0f, 0.84f, 0.0f);
                }
            }
        }
    }

    void drawPlayer(const Player& player)
    {
        float x = player.getX();
        float y = player.getY();
        float size = player.getSize();

        drawFilledRect(x + 3.0f, y - 3.0f, size, size, 0.0f, 0.0f, 0.0f, 0.22f);
        drawFilledRect(x, y, size, size, Colors::PLAYER_R, Colors::PLAYER_G, Colors::PLAYER_B);
        drawFilledRect(x + 5.0f, y + 5.0f, size - 10.0f, size - 10.0f,
                     Colors::PLAYER_LIGHT_R, Colors::PLAYER_LIGHT_G, Colors::PLAYER_LIGHT_B);
        drawRectOutline(x, y, size, size, 2.0f, 0.95f, 1.0f, 0.97f, 0.9f);
    }

    float getGameCanvasWidth()
    {
        return Config::BOARD_WIDTH + Config::SIDE_PANEL_WIDTH + 52.0f;
    }

    float getGameCanvasHeight()
    {
        return Config::BOARD_HEIGHT + 40.0f;
    }

    float getGameCanvasX(int windowWidth)
    {
        float x = (windowWidth - getGameCanvasWidth()) / 2.0f;
        return x < 16.0f ? 16.0f : x;
    }

    float getGameCanvasY(int windowHeight)
    {
        float y = (windowHeight - getGameCanvasHeight()) / 2.0f;
        return y < 16.0f ? 16.0f : y;
    }

    float getBoardOriginX(int windowWidth)
    {
        return getGameCanvasX(windowWidth) + Config::CANVAS_MARGIN;
    }

    float getBoardOriginY(int windowHeight)
    {
        return getGameCanvasY(windowHeight) + Config::CANVAS_MARGIN;
    }

    float getSidebarX(int windowWidth)
    {
        return getBoardOriginX(windowWidth) + Config::BOARD_WIDTH + Config::SIDEBAR_MARGIN;
    }

    float getSidebarY(int windowHeight)
    {
        return getBoardOriginY(windowHeight);
    }
}
