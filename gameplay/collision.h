#ifndef COLLISION_H
#define COLLISION_H

#include "../core/types.h"
#include "maze.h"
#include "player.h"

namespace Collision
{
    bool canMoveTo(const Maze& maze, const GridPos& targetPos);
    bool isAtExit(const Maze& maze, const Player& player);
}

#endif // COLLISION_H
