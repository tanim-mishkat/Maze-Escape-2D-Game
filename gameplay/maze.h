#ifndef MAZE_H
#define MAZE_H

#include "../core/config.h"
#include "../core/types.h"
#include "../data/leveldata.h"
#include <vector>

class Maze
{
public:
    Maze();

    void loadLevel(const LevelDefinition& level);
    void generateProcedural(GridPos startPos, GridPos exitPos, int difficulty);
    void placeRandomTiles(TileType tileType, int count);
    void removeTile(int row, int col);

    TileType getTile(int row, int col) const;
    TileType getTile(const GridPos& pos) const;
    bool isWall(int row, int col) const;
    bool isWall(const GridPos& pos) const;
    bool isBlocking(int row, int col) const;
    bool isBlocking(const GridPos& pos) const;

    GridPos getExitPos() const { return exitPos; }
    const std::vector<GridPos>& getWinningPath() const { return winningPath; }
    bool isInBounds(int row, int col) const;

private:
    int grid[Config::MAZE_ROWS][Config::MAZE_COLS];
    bool protectedPath[Config::MAZE_ROWS][Config::MAZE_COLS];
    GridPos exitPos;
    std::vector<GridPos> winningPath;

    void markWinningPath(const LevelDefinition& level);
    void markWinningPathFromVector();
    bool canUseRandomCell(int row, int col) const;
    int countProtectedNeighbors(int row, int col) const;
};

#endif // MAZE_H
