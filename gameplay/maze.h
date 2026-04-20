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
    void removeTile(int row, int col);
    void setTile(int row, int col, TileType tile);

    TileType getTile(int row, int col) const;
    TileType getTile(const GridPos& pos) const;
    bool isWall(int row, int col) const;
    bool isWall(const GridPos& pos) const;
    bool isBlocking(int row, int col) const;
    bool isBlocking(const GridPos& pos) const;
    bool isWalkable(int row, int col) const;
    int countWalkableNeighbors(int row, int col) const;

    GridPos getExitPos() const { return exitPos; }
    const std::vector<GridPos>& getWinningPath() const { return winningPath; }
    bool isInBounds(int row, int col) const;

private:
    int grid[Config::MAZE_ROWS][Config::MAZE_COLS];
    bool protectedPath[Config::MAZE_ROWS][Config::MAZE_COLS];
    GridPos exitPos;
    std::vector<GridPos> winningPath;

    void markWinningPathFromVector();
};

#endif // MAZE_H
