#ifndef LEVELDATA_H
#define LEVELDATA_H

#include "../core/config.h"

struct LevelDefinition
{
    const char* name;
    unsigned int seed;
    int candidateCount;
    int minSolutionLength;
    int minDeadEnds;
    int keyProgressPercent;
    int clearBonus;
    int trapCount;
    int obstacleCount;
    int parTimeMs;
};

namespace LevelData
{
    extern const LevelDefinition levels[Config::TOTAL_LEVELS];
}

#endif // LEVELDATA_H
