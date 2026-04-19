#ifndef GENERATOR_H
#define GENERATOR_H

#include "../core/config.h"
#include "../core/types.h"
#include <vector>

// Procedural maze generator using DFS with guaranteed path
class MazeGenerator
{
public:
    MazeGenerator();

    // Generate a solvable maze with guaranteed path from start to exit
    // Returns the winning path
    void generate(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                 GridPos startPos, GridPos exitPos,
                 int difficulty,
                 std::vector<GridPos>& outPath);

private:
    bool visited[Config::MAZE_ROWS][Config::MAZE_COLS];
    
    void initializeGrid(int grid[Config::MAZE_ROWS][Config::MAZE_COLS]);
    void carvePath(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                  GridPos current, GridPos target,
                  std::vector<GridPos>& path);
    void addBranches(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                    int branchCount);
    bool isValid(int row, int col) const;
    void shuffleDirections(int directions[4][2], int count);
};

#endif // GENERATOR_H
