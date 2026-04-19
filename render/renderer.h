#ifndef RENDERER_H
#define RENDERER_H

#include "../gameplay/maze.h"
#include "../gameplay/player.h"
#include "../utils/timer.h"

namespace Renderer
{
    // Primitive drawing
    void drawFilledRect(float x, float y, float width, float height,
                       float r, float g, float b, float a = 1.0f);
    void drawRectOutline(float x, float y, float width, float height,
                        float lineWidth, float r, float g, float b, float a = 1.0f);

    // Game element rendering
    void drawFloor();
    void drawMaze(const Maze& maze);
    void drawExit(const Maze& maze, float animationTime, bool hasKey);
    void drawTraps(const Maze& maze, float animationTime);
    void drawObstacles(const Maze& maze);
    void drawKeys(const Maze& maze, float animationTime);
    void drawPlayer(const Player& player);

    // Layout helpers
    float getGameCanvasWidth();
    float getGameCanvasHeight();
    float getGameCanvasX(int windowWidth);
    float getGameCanvasY(int windowHeight);
    float getBoardOriginX(int windowWidth);
    float getBoardOriginY(int windowHeight);
    float getSidebarX(int windowWidth);
    float getSidebarY(int windowHeight);
}

#endif // RENDERER_H
