#include "level.h"

Level::Level() : currentIndex(0), procedural(false)
{
}

void Level::load(int levelIndex)
{
    if (levelIndex < 0 || levelIndex >= Config::TOTAL_LEVELS)
    {
        levelIndex = 0;
    }

    currentIndex = levelIndex;
    procedural = false;
    const LevelSpec& def = LevelData::levels[levelIndex];
    maze.loadLevel(def);
}

void Level::loadProcedural(int levelIndex)
{
    load(levelIndex);
}

const LevelSpec& Level::getDefinition() const
{
    if (currentIndex >= 0 && currentIndex < Config::TOTAL_LEVELS)
    {
        return LevelData::levels[currentIndex];
    }

    return LevelData::levels[0];
}
