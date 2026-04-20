// ============================================================================
// MAZE GENERATION - ANALYSIS FUNCTIONS
// ============================================================================
// This file contains metric computation and analysis functions for maze
// generation. These functions evaluate maze quality by calculating properties
// such as path length, dead ends, turns, and routing difficulty.
//
// Separated from generator.cpp for better code organization and faster
// compilation of the core generation algorithms.
// ============================================================================

#include "generator.h"
#include "maze.h"
#include <queue>
#include <algorithm>

void MazeGenerator::computeDistanceFromCritical(const std::vector<TileType> &grid,
                                                int rows,
                                                int cols,
                                                const std::vector<GridPos> &criticalPath,
                                                std::vector<int> &distances) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    std::queue<GridPos> frontier;

    distances.assign(rows * cols, -1);

    for (std::size_t i = 0; i < criticalPath.size(); i++)
    {
        int index = toIndex(cols, criticalPath[i].row, criticalPath[i].col);
        distances[index] = 0;
        frontier.push(criticalPath[i]);
    }

    while (!frontier.empty())
    {
        GridPos current = frontier.front();
        frontier.pop();

        for (int i = 0; i < 4; i++)
        {
            int nextRow = current.row + directions[i][0];
            int nextCol = current.col + directions[i][1];

            if (!isInBounds(rows, cols, nextRow, nextCol))
            {
                continue;
            }

            int nextIndex = toIndex(cols, nextRow, nextCol);
            if (distances[nextIndex] != -1 || !isWalkableValue(grid[nextIndex]))
            {
                continue;
            }

            distances[nextIndex] = distances[toIndex(cols, current.row, current.col)] + 1;
            frontier.push(GridPos(nextRow, nextCol));
        }
    }
}

int MazeGenerator::countDeadEnds(const std::vector<TileType> &grid,
                                 int rows,
                                 int cols,
                                 const std::vector<GridPos> &criticalPath) const
{
    std::vector<unsigned char> criticalMask(rows * cols, 0);
    int deadEnds = 0;

    for (std::size_t i = 0; i < criticalPath.size(); i++)
    {
        criticalMask[toIndex(cols, criticalPath[i].row, criticalPath[i].col)] = 1;
    }

    for (int row = 1; row < rows - 1; row++)
    {
        for (int col = 1; col < cols - 1; col++)
        {
            int index = toIndex(cols, row, col);
            if (criticalMask[index] != 0 || !isWalkableValue(grid[index]))
            {
                continue;
            }

            if (countWalkableNeighbors(grid, rows, cols, row, col) <= 1)
            {
                deadEnds++;
            }
        }
    }

    return deadEnds;
}

int MazeGenerator::countWalkableNeighbors(const std::vector<TileType> &grid,
                                          int rows,
                                          int cols,
                                          int row,
                                          int col) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int neighbors = 0;

    for (int i = 0; i < 4; i++)
    {
        int nextRow = row + directions[i][0];
        int nextCol = col + directions[i][1];
        if (isInBounds(rows, cols, nextRow, nextCol) && isWalkableValue(grid[toIndex(cols, nextRow, nextCol)]))
        {
            neighbors++;
        }
    }

    return neighbors;
}

int MazeGenerator::countTurns(const std::vector<GridPos> &path) const
{
    if (path.size() < 3)
    {
        return 0;
    }

    int turns = 0;
    int prevRowStep = path[1].row - path[0].row;
    int prevColStep = path[1].col - path[0].col;

    for (std::size_t i = 2; i < path.size(); i++)
    {
        int rowStep = path[i].row - path[i - 1].row;
        int colStep = path[i].col - path[i - 1].col;

        if (rowStep != prevRowStep || colStep != prevColStep)
        {
            turns++;
        }

        prevRowStep = rowStep;
        prevColStep = colStep;
    }

    return turns;
}

int MazeGenerator::computeLongestStraightRun(const std::vector<GridPos> &path) const
{
    if (path.size() < 2)
    {
        return 0;
    }

    int bestRun = 1;
    int currentRun = 1;
    int prevRowStep = path[1].row - path[0].row;
    int prevColStep = path[1].col - path[0].col;

    for (std::size_t i = 2; i < path.size(); i++)
    {
        int rowStep = path[i].row - path[i - 1].row;
        int colStep = path[i].col - path[i - 1].col;

        if (rowStep == prevRowStep && colStep == prevColStep)
        {
            currentRun++;
        }
        else
        {
            currentRun = 1;
        }

        if (currentRun > bestRun)
        {
            bestRun = currentRun;
        }

        prevRowStep = rowStep;
        prevColStep = colStep;
    }

    return bestRun;
}

int MazeGenerator::computeMaxDistance(const std::vector<int> &distances) const
{
    int maxDistance = 0;
    for (std::size_t i = 0; i < distances.size(); i++)
    {
        if (distances[i] > maxDistance)
        {
            maxDistance = distances[i];
        }
    }
    return maxDistance;
}

int MazeGenerator::countCellsAtDistance(const std::vector<int> &distances, int minDistance) const
{
    int count = 0;
    for (std::size_t i = 0; i < distances.size(); i++)
    {
        if (distances[i] >= minDistance)
        {
            count++;
        }
    }
    return count;
}

int MazeGenerator::countCellsInDistanceRange(const std::vector<int> &distances,
                                             int minDistance,
                                             int maxDistance) const
{
    int count = 0;
    for (std::size_t i = 0; i < distances.size(); i++)
    {
        if (distances[i] >= minDistance && distances[i] <= maxDistance)
        {
            count++;
        }
    }
    return count;
}

int MazeGenerator::countDecisionJunctions(const std::vector<TileType> &grid,
                                          int rows,
                                          int cols) const
{
    int count = 0;
    for (int row = 1; row < rows - 1; row++)
    {
        for (int col = 1; col < cols - 1; col++)
        {
            if (!isNodeCell(GridPos(row, col)) || !isWalkableValue(grid[toIndex(cols, row, col)]))
            {
                continue;
            }

            if (countWalkableNeighbors(grid, rows, cols, row, col) >= 3)
            {
                count++;
            }
        }
    }

    return count;
}

int MazeGenerator::shortestPathLength(const std::vector<TileType> &grid,
                                      int rows,
                                      int cols,
                                      const GridPos &start,
                                      const GridPos &target) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    std::queue<GridPos> frontier;
    std::vector<int> distances(rows * cols, -1);

    frontier.push(start);
    distances[toIndex(cols, start.row, start.col)] = 0;

    while (!frontier.empty())
    {
        GridPos current = frontier.front();
        frontier.pop();

        if (current.row == target.row && current.col == target.col)
        {
            return distances[toIndex(cols, current.row, current.col)];
        }

        for (int i = 0; i < 4; i++)
        {
            int nextRow = current.row + directions[i][0];
            int nextCol = current.col + directions[i][1];
            if (!isInBounds(rows, cols, nextRow, nextCol))
            {
                continue;
            }

            int index = toIndex(cols, nextRow, nextCol);
            if (distances[index] != -1 || !isWalkableValue(grid[index]))
            {
                continue;
            }

            distances[index] = distances[toIndex(cols, current.row, current.col)] + 1;
            frontier.push(GridPos(nextRow, nextCol));
        }
    }

    return -1;
}

bool MazeGenerator::isWalkableValue(TileType tileValue) const
{
    return tileValue != TILE_WALL;
}

bool MazeGenerator::isInBounds(int rows, int cols, int row, int col) const
{
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

bool MazeGenerator::isNodeCell(const GridPos &pos) const
{
    return pos.row % 2 == 1 && pos.col % 2 == 1;
}
