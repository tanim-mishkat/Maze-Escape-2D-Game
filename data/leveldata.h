#ifndef LEVELDATA_H
#define LEVELDATA_H

#include "../core/config.h"

struct LevelDefinition
{
    const char* name;
    const int (*baseMaze)[Config::MAZE_COLS];
    const int (*winningPath)[2];
    int pathLength;
    int exitRow;
    int exitCol;
    int obstacleCount;
    int trapCount;
    int clearBonus;
};

namespace LevelData
{
    // Maze definitions
    extern const int easyMaze[Config::MAZE_ROWS][Config::MAZE_COLS];
    extern const int mediumMaze[Config::MAZE_ROWS][Config::MAZE_COLS];
    extern const int hardMaze[Config::MAZE_ROWS][Config::MAZE_COLS];

    // Winning paths
    extern const int easyPath[][2];
    extern const int mediumPath[][2];
    extern const int hardPath[][2];

    extern const int easyPathLength;
    extern const int mediumPathLength;
    extern const int hardPathLength;

    // Level definitions array
    extern const LevelDefinition levels[Config::TOTAL_LEVELS];
}

#endif // LEVELDATA_H
