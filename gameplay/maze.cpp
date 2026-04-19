#include "maze.h"
#include "generator.h"
#include <cstdlib>

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
    // Copy base maze
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            grid[row][col] = level.baseMaze[row][col];
        }
    }

    exitPos = GridPos(level.exitRow, level.exitCol);
    markWinningPath(level);
}

void Maze::generateProcedural(GridPos startPos, GridPos exitPos, int difficulty)
{
    this->exitPos = exitPos;
    winningPath.clear();
    
    MazeGenerator generator;
    generator.generate(grid, startPos, exitPos, difficulty, winningPath);
    
    markWinningPathFromVector();
}

void Maze::markWinningPath(const LevelDefinition& level)
{
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            protectedPath[row][col] = false;
        }
    }

    for (int i = 0; i < level.pathLength; i++)
    {
        int row = level.winningPath[i][0];
        int col = level.winningPath[i][1];
        protectedPath[row][col] = true;
    }
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

    for (size_t i = 0; i < winningPath.size(); i++)
    {
        protectedPath[winningPath[i].row][winningPath[i].col] = true;
    }
}

void Maze::placeRandomTiles(TileType tileType, int count)
{
    int placed = 0;
    int attempts = 0;

    while (placed < count && attempts < 500)
    {
        int row = 1 + std::rand() % (Config::MAZE_ROWS - 2);
        int col = 1 + std::rand() % (Config::MAZE_COLS - 2);
        attempts++;

        if (!canUseRandomCell(row, col))
        {
            continue;
        }

        if (tileType == TILE_OBSTACLE && countProtectedNeighbors(row, col) > 0)
        {
            continue;
        }

        grid[row][col] = tileType;
        placed++;
    }
}

void Maze::removeTile(int row, int col)
{
    if (row >= 0 && row < Config::MAZE_ROWS && col >= 0 && col < Config::MAZE_COLS)
    {
        grid[row][col] = TILE_PATH;
    }
}

bool Maze::canUseRandomCell(int row, int col) const
{
    if (grid[row][col] != TILE_PATH)
    {
        return false;
    }
    if (row == Config::START_ROW && col == Config::START_COL)
    {
        return false;
    }
    if (row == exitPos.row && col == exitPos.col)
    {
        return false;
    }
    if (protectedPath[row][col])
    {
        return false;
    }
    return true;
}

int Maze::countProtectedNeighbors(int row, int col) const
{
    int protectedNeighbors = 0;
    const int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (int i = 0; i < 4; i++)
    {
        int nextRow = row + directions[i][0];
        int nextCol = col + directions[i][1];

        if (nextRow >= 0 && nextRow < Config::MAZE_ROWS &&
            nextCol >= 0 && nextCol < Config::MAZE_COLS &&
            protectedPath[nextRow][nextCol])
        {
            protectedNeighbors++;
        }
    }

    return protectedNeighbors;
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

bool Maze::isInBounds(int row, int col) const
{
    return row >= 0 && row < Config::MAZE_ROWS && col >= 0 && col < Config::MAZE_COLS;
}
