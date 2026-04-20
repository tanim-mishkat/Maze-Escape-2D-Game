#ifndef RENDERER_H
#define RENDERER_H

#include "../gameplay/maze.h"
#include "../gameplay/player.h"

namespace Renderer
{
    void drawFilledRect(float x, float y, float width, float height,
                        float r, float g, float b, float a = 1.0f);
    void drawRectOutline(float x, float y, float width, float height,
                         float lineWidth, float r, float g, float b, float a = 1.0f);

    void drawFloor(const BoardMetrics& metrics);
    void drawMaze(const Maze& maze);
    void drawExit(const Maze& maze, float animationTime);
    void drawPlayer(const Player& player, const BoardMetrics& metrics);

    float getGameCanvasWidth(const BoardMetrics& metrics);
    float getGameCanvasHeight(const BoardMetrics& metrics);
    float getGameCanvasX(int windowWidth, const BoardMetrics& metrics);
    float getGameCanvasY(int windowHeight, const BoardMetrics& metrics);
    float getBoardOriginX(int windowWidth, const BoardMetrics& metrics);
    float getBoardOriginY(int windowHeight, const BoardMetrics& metrics);
    float getSidebarX(int windowWidth, const BoardMetrics& metrics);
    float getSidebarY(int windowHeight, const BoardMetrics& metrics);
}

#endif // RENDERER_H
