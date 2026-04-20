#ifndef COORDS_H
#define COORDS_H

#include "../core/types.h"
#include "../core/config.h"

namespace Coords
{
    ScreenPos gridToScreen(const BoardMetrics& metrics, const GridPos& grid);
    ScreenPos gridToScreen(const BoardMetrics& metrics, int row, int col);
    GridPos screenToGrid(const BoardMetrics& metrics, const ScreenPos& screen);
    GridPos screenToGrid(const BoardMetrics& metrics, float x, float y);
}

#endif // COORDS_H
