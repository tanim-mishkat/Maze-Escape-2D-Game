#ifndef COLLISION_H
#define COLLISION_H

#include "../core/types.h"
#include "maze.h"
#include "player.h"

namespace Collision
{
    bool canMoveTo(const Maze& maze, const GridPos& targetPos);
    bool isAtTrap(const Maze& maze, const Player& player);
    bool isAtExit(const Maze& maze, const Player& player, bool hasKey);
    bool isAtKey(const Maze& maze, const Player& player);
}

#endif // COLLISION_H
