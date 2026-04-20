#ifndef GENERATOR_H
#define GENERATOR_H

#include "../core/types.h"
#include "../data/leveldata.h"
#include <vector>

class Maze;

class MazeGenerator
{
public:
    void generate(Maze& maze,
                  const LevelSpec& definition,
                  GridPos& outExitPos,
                  std::vector<GridPos>& outPath) const;

private:
    struct MazeCandidate
    {
        std::vector<TileType> grid;
        GridPos exitPos;
        std::vector<GridPos> criticalPath;
        int criticalPathLength;
        int deadEnds;
        int turnCount;
        int longestStraightRun;
        int routeDetour;
        int nearCriticalBranches;
        int junctionCount;
        int maxBranchDepth;
        int secondOrderBranches;
        int score;
    };

    struct SpatialCandidate
    {
        GridPos pos;
        int distanceFromCritical;
        int degree;
        bool roomCell;
    };

    void initializeGrid(std::vector<TileType>& grid, int rows, int cols) const;
    void generateLayout(MazeCandidate& outCandidate,
                        const LevelSpec& definition,
                        unsigned int seed) const;
    void carvePerfectMaze(std::vector<TileType>& grid,
                          const LevelSpec& definition,
                          unsigned int seed) const;
    void buildCriticalPath(const std::vector<TileType>& grid,
                           const LevelSpec& definition,
                           GridPos& outExitPos,
                           std::vector<GridPos>& outPath) const;
    void enhanceLayout(std::vector<TileType>& grid,
                       const LevelSpec& definition,
                       const std::vector<GridPos>& criticalPath,
                       const GridPos& exitPos,
                       unsigned int seed) const;
    int carveRooms(std::vector<TileType>& grid,
                   const LevelSpec& definition,
                   const std::vector<unsigned char>& criticalMask,
                   std::vector<unsigned char>& roomMask,
                   std::vector<unsigned char>& doorwayMask,
                   unsigned int seed) const;
    int addLoops(std::vector<TileType>& grid,
                 const LevelSpec& definition,
                 const std::vector<unsigned char>& criticalMask,
                 const std::vector<unsigned char>& roomMask,
                 const std::vector<unsigned char>& doorwayMask,
                 unsigned int seed) const;
    int addDecisionJunctions(std::vector<TileType>& grid,
                             const LevelSpec& definition,
                             const std::vector<GridPos>& criticalPath,
                             const GridPos& exitPos,
                             const std::vector<unsigned char>& roomMask,
                             const std::vector<unsigned char>& doorwayMask,
                             unsigned int seed) const;
    void computeDistanceFromCritical(const std::vector<TileType>& grid,
                                     int rows,
                                     int cols,
                                     const std::vector<GridPos>& criticalPath,
                                     std::vector<int>& distances) const;
    int countDeadEnds(const std::vector<TileType>& grid,
                      int rows,
                      int cols,
                      const std::vector<GridPos>& criticalPath) const;
    int countWalkableNeighbors(const std::vector<TileType>& grid,
                               int rows,
                               int cols,
                               int row,
                               int col) const;
    int countTurns(const std::vector<GridPos>& path) const;
    int computeLongestStraightRun(const std::vector<GridPos>& path) const;
    int computeMaxDistance(const std::vector<int>& distances) const;
    int countCellsAtDistance(const std::vector<int>& distances, int minDistance) const;
    int countCellsInDistanceRange(const std::vector<int>& distances,
                                  int minDistance,
                                  int maxDistance) const;
    int countDecisionJunctions(const std::vector<TileType>& grid,
                               int rows,
                               int cols) const;
    int shortestPathLength(const std::vector<TileType>& grid,
                           int rows,
                           int cols,
                           const GridPos& start,
                           const GridPos& target) const;

    bool tryCarveRoom(std::vector<TileType>& grid,
                      int rows,
                      int cols,
                      const GridPos& doorway,
                      int directionRow,
                      int directionCol,
                      int roomRows,
                      int roomCols,
                      std::vector<unsigned char>& roomMask,
                      std::vector<unsigned char>& doorwayMask) const;
    bool isWalkableValue(TileType tileValue) const;
    bool isInBounds(int rows, int cols, int row, int col) const;
    bool isNodeCell(const GridPos& pos) const;
    int toIndex(int cols, int row, int col) const { return row * cols + col; }
    GridPos gridToNode(const GridPos& gridPos) const;
    GridPos nodeToGrid(const GridPos& nodePos) const;
};

#endif // GENERATOR_H
