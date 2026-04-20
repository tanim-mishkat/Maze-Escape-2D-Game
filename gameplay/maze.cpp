#include "maze.h"
#include "generator.h"

Maze::Maze()
    : rows(0),
      cols(0),
      startPos(0, 0),
      exitPos(0, 0)
{
}

void Maze::resize(int newRows, int newCols)
{
    rows = newRows;
    cols = newCols;
    metrics = Config::makeBoardMetrics(rows, cols);
    grid.assign(rows * cols, TILE_WALL);
    protectedPath.assign(rows * cols, 0);
    winningPath.clear();
}

void Maze::loadLevel(const LevelSpec& level)
{
    resize(level.rows, level.cols);
    startPos = level.startPos;
    exitPos = level.startPos;

    MazeGenerator generator;
    generator.generate(*this, level, exitPos, winningPath);
    markWinningPathFromVector();
}

void Maze::markWinningPathFromVector()
{
    protectedPath.assign(rows * cols, 0);

    for (std::size_t i = 0; i < winningPath.size(); i++)
    {
        if (isInBounds(winningPath[i].row, winningPath[i].col))
        {
            protectedPath[toIndex(winningPath[i].row, winningPath[i].col)] = 1;
        }
    }
}

void Maze::removeTile(int row, int col)
{
    if (isInBounds(row, col))
    {
        grid[toIndex(row, col)] = TILE_PATH;
    }
}

void Maze::setTile(int row, int col, TileType tile)
{
    if (isInBounds(row, col))
    {
        grid[toIndex(row, col)] = tile;
    }
}

TileType Maze::getTile(int row, int col) const
{
    if (!isInBounds(row, col))
    {
        return TILE_WALL;
    }

    return grid[toIndex(row, col)];
}

TileType Maze::getTile(const GridPos& pos) const
{
    return getTile(pos.row, pos.col);
}

bool Maze::isWall(int row, int col) const
{
    return getTile(row, col) == TILE_WALL;
}

bool Maze::isWall(const GridPos& pos) const
{
    return isWall(pos.row, pos.col);
}

bool Maze::isBlocking(int row, int col) const
{
    TileType tile = getTile(row, col);
    return tile == TILE_WALL;
}

bool Maze::isBlocking(const GridPos& pos) const
{
    return isBlocking(pos.row, pos.col);
}

bool Maze::isWalkable(int row, int col) const
{
    return !isBlocking(row, col);
}

int Maze::countWalkableNeighbors(int row, int col) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int neighbors = 0;

    for (int i = 0; i < 4; i++)
    {
        int nextRow = row + directions[i][0];
        int nextCol = col + directions[i][1];

        if (isInBounds(nextRow, nextCol) && isWalkable(nextRow, nextCol))
        {
            neighbors++;
        }
    }

    return neighbors;
}

bool Maze::isProtected(int row, int col) const
{
    if (!isInBounds(row, col))
    {
        return false;
    }

    return protectedPath[toIndex(row, col)] != 0;
}

bool Maze::isInBounds(int row, int col) const
{
    return row >= 0 && row < rows && col >= 0 && col < cols;
}
