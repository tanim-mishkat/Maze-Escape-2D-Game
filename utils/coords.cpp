#include "coords.h"

namespace Coords
{
    ScreenPos gridToScreen(const GridPos& grid)
    {
        return gridToScreen(grid.row, grid.col);
    }

    ScreenPos gridToScreen(int row, int col)
    {
        float x = col * Config::TILE_SIZE;
        float y = Config::BOARD_HEIGHT - (row + 1) * Config::TILE_SIZE;
        return ScreenPos(x, y);
    }

    GridPos screenToGrid(const ScreenPos& screen)
    {
        return screenToGrid(screen.x, screen.y);
    }

    GridPos screenToGrid(float x, float y)
    {
        int col = static_cast<int>(x / Config::TILE_SIZE);
        int row = Config::MAZE_ROWS - 1 - static_cast<int>(y / Config::TILE_SIZE);
        return GridPos(row, col);
    }

    GridPos playerScreenToGrid(float playerX, float playerY, float playerSize)
    {
        float centerX = playerX + playerSize / 2.0f;
        float centerY = playerY + playerSize / 2.0f;
        return screenToGrid(centerX, centerY);
    }

    ScreenPos playerGridToScreen(const GridPos& grid, float padding)
    {
        return playerGridToScreen(grid.row, grid.col, padding);
    }

    ScreenPos playerGridToScreen(int row, int col, float padding)
    {
        float x = col * Config::TILE_SIZE + padding;
        float y = (Config::MAZE_ROWS - 1 - row) * Config::TILE_SIZE + padding;
        return ScreenPos(x, y);
    }
}
