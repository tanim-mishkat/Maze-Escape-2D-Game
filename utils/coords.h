#ifndef COORDS_H
#define COORDS_H

#include "../core/types.h"
#include "../core/config.h"

namespace Coords
{
    // Convert grid position to screen position (top-left of tile)
    ScreenPos gridToScreen(const GridPos& grid);
    ScreenPos gridToScreen(int row, int col);

    // Convert screen position to grid position
    GridPos screenToGrid(const ScreenPos& screen);
    GridPos screenToGrid(float x, float y);

    // Get player's grid position from screen position (center-based)
    GridPos playerScreenToGrid(float playerX, float playerY, float playerSize);

    // Get player's screen position from grid position (with padding)
    ScreenPos playerGridToScreen(const GridPos& grid, float padding);
    ScreenPos playerGridToScreen(int row, int col, float padding);
}

#endif // COORDS_H
