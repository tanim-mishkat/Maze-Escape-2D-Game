#include "maze.h"
#include "generator.h"

Maze::Maze() : exitPos(0, 0)
{
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            grid[row][col] = TILE_WALL;
            protectedPath[row][col] = false;
        }
    }
}

void Maze::loadLevel(const LevelDefinition& level)
{
    winningPath.clear();

    MazeGenerator generator;
    generator.generate(grid,
                       GridPos(Config::START_ROW, Config::START_COL),
                       level,
                       exitPos,
                       winningPath);
    markWinningPathFromVector();
}

void Maze::markWinningPathFromVector()
{
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            protectedPath[row][col] = false;
        }
    }

    for (std::size_t i = 0; i < winningPath.size(); i++)
    {
        protectedPath[winningPath[i].row][winningPath[i].col] = true;
    }
}

void Maze::removeTile(int row, int col)
{
    if (row >= 0 && row < Config::MAZE_ROWS && col >= 0 && col < Config::MAZE_COLS)
    {
        grid[row][col] = TILE_PATH;
    }
}

void Maze::setTile(int row, int col, TileType tile)
{
    if (row >= 0 && row < Config::MAZE_ROWS && col >= 0 && col < Config::MAZE_COLS)
    {
        grid[row][col] = tile;
    }
}

TileType Maze::getTile(int row, int col) const
{
    if (row < 0 || row >= Config::MAZE_ROWS || col < 0 || col >= Config::MAZE_COLS)
    {
        return TILE_WALL;
    }
    return static_cast<TileType>(grid[row][col]);
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
    return tile == TILE_WALL || tile == TILE_OBSTACLE;
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

bool Maze::isInBounds(int row, int col) const
{
    return row >= 0 && row < Config::MAZE_ROWS && col >= 0 && col < Config::MAZE_COLS;
}
