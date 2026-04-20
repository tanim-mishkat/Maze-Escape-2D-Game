#ifndef COORDS_H
#define COORDS_H

#include "../core/types.h"
#include "../core/config.h"

// ============================================================================
// COORDINATE SYSTEMS
// ============================================================================
//
// The game uses two primary coordinate systems:
//
// GRID COORDINATES (logical game space):
//   - Origin (0,0) at top-left of maze
//   - Row increases downward (0 = top row)
//   - Col increases rightward (0 = leftmost column)
//   - Used internally for maze logic, collision, and pathfinding
//
// SCREEN COORDINATES (OpenGL rendering space):
//   - Origin (0,0) at bottom-left of screen (OpenGL standard)
//   - X increases rightward (same as grid column)
//   - Y increases upward (OPPOSITE of grid row - note the flip!)
//   - Used for all OpenGL drawing and mouse input
//
// COORDINATE CONVERSION:
//   Grid (row, col) → Screen (x, y):
//     - x = boardX + col * tileSize
//     - y = boardY + (rows - 1 - row) * tileSize  ← ROW FLIP here
//
//   Screen (x, y) → Grid (row, col):
//     - col = (x - boardX) / tileSize
//     - row = rows - 1 - ((y - boardY) / tileSize)  ← ROW FLIP here
//
// KEY INSIGHT: The row coordinate must be flipped when converting between
// grid space (where row=0 is at the top) and screen space (where y=0 is at
// the bottom). This is critical for correct rendering and input handling.
//
// ============================================================================

namespace Coords
{
    ScreenPos gridToScreen(const BoardMetrics &metrics, const GridPos &grid);
    ScreenPos gridToScreen(const BoardMetrics &metrics, int row, int col);
    GridPos screenToGrid(const BoardMetrics &metrics, const ScreenPos &screen);
    GridPos screenToGrid(const BoardMetrics &metrics, float x, float y);
}

#endif // COORDS_H
