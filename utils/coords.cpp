#include "coords.h"

namespace Coords
{
    ScreenPos gridToScreen(const BoardMetrics& metrics, const GridPos& grid)
    {
        return gridToScreen(metrics, grid.row, grid.col);
    }

    ScreenPos gridToScreen(const BoardMetrics& metrics, int row, int col)
    {
        float x = static_cast<float>(col) * metrics.tileSize;
        float y = metrics.boardHeight - static_cast<float>(row + 1) * metrics.tileSize;
        return ScreenPos(x, y);
    }

    GridPos screenToGrid(const BoardMetrics& metrics, const ScreenPos& screen)
    {
        return screenToGrid(metrics, screen.x, screen.y);
    }

    GridPos screenToGrid(const BoardMetrics& metrics, float x, float y)
    {
        int col = static_cast<int>(x / metrics.tileSize);
        int row = metrics.rows - 1 - static_cast<int>(y / metrics.tileSize);
        return GridPos(row, col);
    }
}
