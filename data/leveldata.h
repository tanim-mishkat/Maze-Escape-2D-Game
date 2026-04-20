#ifndef LEVELDATA_H
#define LEVELDATA_H

#include "../core/config.h"
#include "../core/types.h"

struct LevelSpec
{
    const char* name;
    int rows;
    int cols;
    GridPos startPos;
    GridPos exitTarget;
    unsigned int seed;
    int candidateCount;
    int minCriticalPath;
    int minDeadEnds;
    int roomBudget;
    int roomMinRows;
    int roomMaxRows;
    int roomMinCols;
    int roomMaxCols;
    int loopBudget;
    int junctionTarget;
    int parTimeMs;
};

namespace LevelData
{
    extern const LevelSpec levels[Config::TOTAL_LEVELS];
}

#endif // LEVELDATA_H
