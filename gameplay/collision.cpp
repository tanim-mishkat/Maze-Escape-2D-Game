#include "collision.h"

namespace Collision
{
    bool canMoveTo(const Maze& maze, const GridPos& targetPos)
    {
        return !maze.isBlocking(targetPos);
    }

    bool isAtTrap(const Maze& maze, const Player& player)
    {
        GridPos playerPos = player.getGridPos();
        return maze.getTile(playerPos) == TILE_TRAP;
    }

    bool isAtExit(const Maze& maze, const Player& player, bool hasKey)
    {
        GridPos playerPos = player.getGridPos();
        GridPos exitPos = maze.getExitPos();
        
        if (playerPos.row != exitPos.row || playerPos.col != exitPos.col)
        {
            return false;
        }
        
        // Check if exit is locked
        TileType exitTile = maze.getTile(exitPos);
        if (exitTile == TILE_LOCKED_EXIT)
        {
            return hasKey;
        }
        
        return true;
    }
    
    bool isAtKey(const Maze& maze, const Player& player)
    {
        GridPos playerPos = player.getGridPos();
        return maze.getTile(playerPos) == TILE_KEY;
    }
}
