#include "collision.h"

namespace Collision
{
    bool canMoveTo(const Maze& maze, const GridPos& targetPos)
    {
        return !maze.isBlocking(targetPos);
    }

    bool isAtExit(const Maze& maze, const Player& player)
    {
        GridPos playerPos = player.getGridPos();
        GridPos exitPos = maze.getExitPos();
        return playerPos.row == exitPos.row && playerPos.col == exitPos.col;
    }
}
