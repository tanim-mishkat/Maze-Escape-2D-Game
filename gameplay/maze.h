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

    void loadLevel(const LevelSpec& level);
    void removeTile(int row, int col);
    void setTile(int row, int col, TileType tile);
    void resize(int rows, int cols);

    TileType getTile(int row, int col) const;
    TileType getTile(const GridPos& pos) const;
    bool isWall(int row, int col) const;
    bool isWall(const GridPos& pos) const;
    bool isBlocking(int row, int col) const;
    bool isBlocking(const GridPos& pos) const;
    bool isWalkable(int row, int col) const;
    int countWalkableNeighbors(int row, int col) const;
    bool isProtected(int row, int col) const;

    int getRows() const { return rows; }
    int getCols() const { return cols; }
    GridPos getStartPos() const { return startPos; }
    GridPos getExitPos() const { return exitPos; }
    const BoardMetrics& getMetrics() const { return metrics; }
    const std::vector<GridPos>& getWinningPath() const { return winningPath; }
    bool isInBounds(int row, int col) const;

    std::vector<TileType>& getMutableGrid() { return grid; }
    const std::vector<TileType>& getGrid() const { return grid; }

private:
    int rows;
    int cols;
    GridPos startPos;
    GridPos exitPos;
    BoardMetrics metrics;
    std::vector<TileType> grid;
    std::vector<unsigned char> protectedPath;
    std::vector<GridPos> winningPath;

    int toIndex(int row, int col) const { return row * cols + col; }
    void markWinningPathFromVector();
};

#endif // MAZE_H
