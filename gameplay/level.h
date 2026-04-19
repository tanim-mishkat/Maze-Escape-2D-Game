#ifndef LEVEL_H
#define LEVEL_H

#include "../data/leveldata.h"
#include "maze.h"

class Level
{
public:
    Level();

    void load(int levelIndex);
    void loadProcedural(int levelIndex);
    const LevelDefinition& getDefinition() const;
    int getIndex() const { return currentIndex; }
    Maze& getMaze() { return maze; }
    const Maze& getMaze() const { return maze; }
    bool isProcedural() const { return procedural; }

private:
    Maze maze;
    int currentIndex;
    bool procedural;
};

#endif // LEVEL_H
