#include "level.h"

Level::Level() : currentIndex(0), procedural(false)
{
}

void Level::load(int levelIndex)
{
    currentIndex = levelIndex;
    procedural = false;
    const LevelDefinition& def = LevelData::levels[levelIndex];
    maze.loadLevel(def);
}

void Level::loadProcedural(int levelIndex)
{
    load(levelIndex);
}

const LevelDefinition& Level::getDefinition() const
{
    if (currentIndex >= 0 && currentIndex < Config::TOTAL_LEVELS)
    {
        return LevelData::levels[currentIndex];
    }
    // Return first level as safe fallback
    return LevelData::levels[0];
}
