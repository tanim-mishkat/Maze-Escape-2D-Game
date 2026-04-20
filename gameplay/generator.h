#ifndef GENERATOR_H
#define GENERATOR_H

#include "../core/config.h"
#include "../core/types.h"
#include "../data/leveldata.h"
#include <vector>

class MazeGenerator
{
public:
    MazeGenerator();

    void generate(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                  GridPos startPos,
                  const LevelDefinition& definition,
                  GridPos& outExitPos,
                  std::vector<GridPos>& outPath);

private:
    static const int NODE_ROWS = (Config::MAZE_ROWS - 1) / 2;
    static const int NODE_COLS = (Config::MAZE_COLS - 1) / 2;

    struct MazeCandidate
    {
        int grid[Config::MAZE_ROWS][Config::MAZE_COLS];
        GridPos exitPos;
        std::vector<GridPos> solutionPath;
        int solutionLength;
        int deadEndCount;
        int score;
    };

    struct CellCandidate
    {
        GridPos pos;
        int distanceFromSolution;
    };

    bool visited[NODE_ROWS][NODE_COLS];

    void initializeGrid(int grid[Config::MAZE_ROWS][Config::MAZE_COLS]);
    void generateLayout(MazeCandidate& outCandidate,
                        const GridPos& startPos,
                        unsigned int seed);
    void carvePerfectMaze(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                          const GridPos& startNode,
                          unsigned int seed);
    void buildSolutionPath(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                           const GridPos& startNode,
                           GridPos& outExitNode,
                           std::vector<GridPos>& outPath) const;
    void applyObjectivesAndHazards(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                   const std::vector<GridPos>& solutionPath,
                                   const GridPos& exitPos,
                                   const LevelDefinition& definition,
                                   unsigned int seed) const;
    void computeDistanceFromSolution(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                     const std::vector<GridPos>& solutionPath,
                                     int distances[Config::MAZE_ROWS][Config::MAZE_COLS]) const;
    GridPos chooseKeyCell(const std::vector<GridPos>& solutionPath,
                          int keyProgressPercent) const;
    int countDeadEnds(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                      const std::vector<GridPos>& solutionPath) const;
    int countWalkableNeighbors(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                               int row,
                               int col) const;
    bool isNodeValid(int row, int col) const;
    bool isWalkableValue(int tileValue) const;
    GridPos gridToNode(const GridPos& gridPos) const;
    GridPos nodeToGrid(const GridPos& nodePos) const;
};

#endif // GENERATOR_H
