#include "generator.h"
#include <cstdlib>
#include <cstring>

MazeGenerator::MazeGenerator()
{
    std::memset(visited, 0, sizeof(visited));
}

void MazeGenerator::generate(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                             GridPos startPos, GridPos exitPos,
                             int difficulty,
                             std::vector<GridPos>& outPath)
{
    // Initialize grid with walls
    initializeGrid(grid);
    
    // Clear visited array
    std::memset(visited, 0, sizeof(visited));
    
    // Clear output path
    outPath.clear();
    
    // Carve guaranteed path from start to exit using DFS
    carvePath(grid, startPos, exitPos, outPath);
    
    // Add additional branches based on difficulty
    int branchCount = 3 + difficulty * 2;
    addBranches(grid, branchCount);
    
    // Ensure borders are walls
    for (int col = 0; col < Config::MAZE_COLS; col++)
    {
        grid[0][col] = TILE_WALL;
        grid[Config::MAZE_ROWS - 1][col] = TILE_WALL;
    }
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        grid[row][0] = TILE_WALL;
        grid[row][Config::MAZE_COLS - 1] = TILE_WALL;
    }
}

void MazeGenerator::initializeGrid(int grid[Config::MAZE_ROWS][Config::MAZE_COLS])
{
    // Fill with walls
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            grid[row][col] = TILE_WALL;
        }
    }
}

void MazeGenerator::carvePath(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                              GridPos current, GridPos target,
                              std::vector<GridPos>& path)
{
    // Mark current as visited and carve it
    visited[current.row][current.col] = true;
    grid[current.row][current.col] = TILE_PATH;
    path.push_back(current);
    
    // If we reached target, we're done
    if (current.row == target.row && current.col == target.col)
    {
        return;
    }
    
    // Try to move toward target with some randomness
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    // Bias toward target
    int bestDir = -1;
    int bestDist = 999999;
    
    for (int i = 0; i < 4; i++)
    {
        int newRow = current.row + directions[i][0];
        int newCol = current.col + directions[i][1];
        
        if (!isValid(newRow, newCol) || visited[newRow][newCol])
        {
            continue;
        }
        
        int dist = abs(newRow - target.row) + abs(newCol - target.col);
        if (dist < bestDist)
        {
            bestDist = dist;
            bestDir = i;
        }
    }
    
    // Move in best direction if found
    if (bestDir != -1)
    {
        GridPos next(current.row + directions[bestDir][0],
                    current.col + directions[bestDir][1]);
        carvePath(grid, next, target, path);
    }
    else
    {
        // Backtrack - try any unvisited neighbor
        shuffleDirections(directions, 4);
        for (int i = 0; i < 4; i++)
        {
            int newRow = current.row + directions[i][0];
            int newCol = current.col + directions[i][1];
            
            if (isValid(newRow, newCol) && !visited[newRow][newCol])
            {
                GridPos next(newRow, newCol);
                carvePath(grid, next, target, path);
                return;
            }
        }
    }
}

void MazeGenerator::addBranches(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                               int branchCount)
{
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (int b = 0; b < branchCount; b++)
    {
        // Find a random path cell to branch from
        int attempts = 0;
        while (attempts < 50)
        {
            int row = 1 + std::rand() % (Config::MAZE_ROWS - 2);
            int col = 1 + std::rand() % (Config::MAZE_COLS - 2);
            attempts++;
            
            if (grid[row][col] != TILE_PATH)
            {
                continue;
            }

            // Try to carve in a random direction
            int dirOrder[4] = {0, 1, 2, 3};
            for (int i = 3; i > 0; i--)
            {
                int j = std::rand() % (i + 1);
                int temp = dirOrder[i];
                dirOrder[i] = dirOrder[j];
                dirOrder[j] = temp;
            }
            
            for (int i = 0; i < 4; i++)
            {
                int dir = dirOrder[i];
                int newRow = row + directions[dir][0];
                int newCol = col + directions[dir][1];
                
                if (!isValid(newRow, newCol) || grid[newRow][newCol] != TILE_WALL)
                {
                    continue;
                }
                
                // Carve a short branch (1-3 tiles)
                int branchLength = 1 + std::rand() % 3;
                int curRow = newRow;
                int curCol = newCol;
                
                for (int len = 0; len < branchLength; len++)
                {
                    if (isValid(curRow, curCol) && grid[curRow][curCol] == TILE_WALL)
                    {
                        grid[curRow][curCol] = TILE_PATH;
                        curRow += directions[dir][0];
                        curCol += directions[dir][1];
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            }
            break;
        }
    }
}

bool MazeGenerator::isValid(int row, int col) const
{
    return row > 0 && row < Config::MAZE_ROWS - 1 &&
           col > 0 && col < Config::MAZE_COLS - 1;
}

void MazeGenerator::shuffleDirections(int directions[4][2], int count)
{
    for (int i = count - 1; i > 0; i--)
    {
        int j = std::rand() % (i + 1);
        int temp0 = directions[i][0];
        int temp1 = directions[i][1];
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = temp0;
        directions[j][1] = temp1;
    }
}
