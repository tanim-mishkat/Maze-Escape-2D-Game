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
    maze.placeRandomTiles(TILE_TRAP, def.trapCount);
    maze.placeRandomTiles(TILE_OBSTACLE, def.obstacleCount);
}

void Level::loadProcedural(int levelIndex)
{
    currentIndex = levelIndex;
    procedural = true;
    
    // Generate procedural maze
    GridPos startPos(Config::START_ROW, Config::START_COL);
    GridPos exitPos(1, Config::MAZE_COLS - 2);
    
    maze.generateProcedural(startPos, exitPos, levelIndex);
    
    // Place traps and obstacles based on difficulty
    int trapCount = 2 + levelIndex;
    int obstacleCount = 1 + levelIndex / 2;
    
    maze.placeRandomTiles(TILE_TRAP, trapCount);
    maze.placeRandomTiles(TILE_OBSTACLE, obstacleCount);
    
    // Place key on harder levels
    if (levelIndex >= 2)
    {
        maze.placeRandomTiles(TILE_KEY, 1);
    }
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
