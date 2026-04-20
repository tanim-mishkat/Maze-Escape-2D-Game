#include "generator.h"
#include "maze.h"
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <random>

// ============================================================================
// MAZE GENERATION ALGORITHM
// ============================================================================
//
// The maze generation process follows these steps:
//
// 1. GENERATION PHASE: Create multiple maze candidate layouts
//    - Uses depth-first search (DFS) to carve perfect mazes (no loops, single path)
//    - Each candidate is uniquely seeded for deterministic variation
//    - Candidates are evaluated for path length, turns, dead ends, and routing difficulty
//
// 2. EVALUATION PHASE: Analyze and score each candidate
//    - Compute critical path (shortest route from start to exit)
//    - Count turns, dead ends, junctions (decision points)
//    - Measure branch depths and secondary branching
//    - Calculate route detour (how much longer than critical path)
//    - Score each candidate using weighted combination of metrics (see Config constants)
//
// 3. SELECTION PHASE: Choose the best qualified candidate
//    - Prioritize candidates meeting difficulty specifications
//    - Break ties using total score
//    - Score weights in Config:: control difficulty tuning:
//      * SCORE_*_WEIGHT constants determine metric importance
//      * Adjust these constants in core/config.h to tune difficulty progression
//
// 4. ENHANCEMENT PHASE: Optionally improve chosen layout
//    - Add strategic rooms (carved open areas)
//    - Add optional loops (shortcuts that don't create trivial solutions)
//    - Add decision junctions to increase complexity
//    - Final layout maintains solvability while increasing difficulty
//
// ============================================================================

void MazeGenerator::generate(Maze &maze,
                             const LevelSpec &definition,
                             GridPos &outExitPos,
                             std::vector<GridPos> &outPath) const
{
    MazeCandidate bestCandidate;
    bool hasBestCandidate = false;
    bool hasQualifiedCandidate = false;

    // Generate and evaluate multiple candidates, select the best one
    for (int attempt = 0; attempt < definition.candidateCount; attempt++)
    {
        MazeCandidate candidate;
        generateLayout(candidate, definition, definition.seed + static_cast<unsigned int>(attempt * 977));

        int minTurnTarget = std::max(8, definition.minCriticalPath / 4);
        int minDetourTarget = std::max(10, definition.minCriticalPath / 3);
        int maxStraightRun = std::max(4, definition.cols / 5);
        bool meetsSpec = candidate.criticalPathLength >= definition.minCriticalPath && candidate.deadEnds >= definition.minDeadEnds && candidate.turnCount >= minTurnTarget && candidate.routeDetour >= minDetourTarget && candidate.longestStraightRun <= maxStraightRun;

        if (!hasBestCandidate || (meetsSpec && !hasQualifiedCandidate) || (meetsSpec == hasQualifiedCandidate && candidate.score > bestCandidate.score))
        {
            bestCandidate = candidate;
            hasBestCandidate = true;
            hasQualifiedCandidate = meetsSpec;
        }
    }

    enhanceLayout(bestCandidate.grid,
                  definition,
                  bestCandidate.criticalPath,
                  bestCandidate.exitPos,
                  definition.seed + 7919u);

    maze.getMutableGrid() = bestCandidate.grid;
    outExitPos = bestCandidate.exitPos;
    outPath = bestCandidate.criticalPath;
}

void MazeGenerator::initializeGrid(std::vector<TileType> &grid, int rows, int cols) const
{
    grid.assign(rows * cols, TILE_WALL);
}

void MazeGenerator::generateLayout(MazeCandidate &outCandidate,
                                   const LevelSpec &definition,
                                   unsigned int seed) const
{
    initializeGrid(outCandidate.grid, definition.rows, definition.cols);
    carvePerfectMaze(outCandidate.grid, definition, seed);
    buildCriticalPath(outCandidate.grid,
                      definition,
                      outCandidate.exitPos,
                      outCandidate.criticalPath);

    std::vector<int> distances;
    computeDistanceFromCritical(outCandidate.grid,
                                definition.rows,
                                definition.cols,
                                outCandidate.criticalPath,
                                distances);

    outCandidate.criticalPathLength = static_cast<int>(outCandidate.criticalPath.size());
    outCandidate.deadEnds = countDeadEnds(outCandidate.grid,
                                          definition.rows,
                                          definition.cols,
                                          outCandidate.criticalPath);
    outCandidate.turnCount = countTurns(outCandidate.criticalPath);
    outCandidate.longestStraightRun = computeLongestStraightRun(outCandidate.criticalPath);
    outCandidate.nearCriticalBranches = countCellsInDistanceRange(distances, 1, 2);
    outCandidate.junctionCount = countDecisionJunctions(outCandidate.grid,
                                                        definition.rows,
                                                        definition.cols);
    outCandidate.maxBranchDepth = computeMaxDistance(distances);
    outCandidate.secondOrderBranches = countCellsAtDistance(distances, 2);
    outCandidate.routeDetour =
        outCandidate.criticalPathLength - (std::abs(outCandidate.exitPos.row - definition.startPos.row) + std::abs(outCandidate.exitPos.col - definition.startPos.col));

    int exitBias = std::abs(outCandidate.exitPos.row - definition.exitTarget.row) + std::abs(outCandidate.exitPos.col - definition.exitTarget.col);
    outCandidate.score =
        outCandidate.criticalPathLength * Config::SCORE_CRITICAL_PATH_WEIGHT +
        outCandidate.turnCount * Config::SCORE_TURN_COUNT_WEIGHT +
        outCandidate.deadEnds * Config::SCORE_DEAD_END_WEIGHT +
        outCandidate.maxBranchDepth * Config::SCORE_MAX_BRANCH_DEPTH_WEIGHT +
        outCandidate.secondOrderBranches * Config::SCORE_SECOND_ORDER_BRANCH_WEIGHT +
        outCandidate.nearCriticalBranches * Config::SCORE_NEAR_CRITICAL_BRANCH_WEIGHT +
        outCandidate.junctionCount * Config::SCORE_JUNCTION_COUNT_WEIGHT +
        outCandidate.routeDetour * Config::SCORE_ROUTE_DETOUR_WEIGHT -
        outCandidate.longestStraightRun * Config::SCORE_LONGEST_STRAIGHT_RUN_WEIGHT -
        exitBias * Config::SCORE_EXIT_BIAS_WEIGHT;
}

void MazeGenerator::carvePerfectMaze(std::vector<TileType> &grid,
                                     const LevelSpec &definition,
                                     unsigned int seed) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    int nodeRows = (definition.rows - 1) / 2;
    int nodeCols = (definition.cols - 1) / 2;
    std::vector<unsigned char> visited(nodeRows * nodeCols, 0);

    std::mt19937 rng(seed);
    std::vector<GridPos> stack;
    GridPos startNode = gridToNode(definition.startPos);
    stack.push_back(startNode);
    visited[toIndex(nodeCols, startNode.row, startNode.col)] = 1;

    GridPos startGrid = nodeToGrid(startNode);
    grid[toIndex(definition.cols, startGrid.row, startGrid.col)] = TILE_PATH;

    while (!stack.empty())
    {
        GridPos currentNode = stack.back();
        std::vector<int> order;

        for (int i = 0; i < 4; i++)
        {
            order.push_back(i);
        }
        std::shuffle(order.begin(), order.end(), rng);

        bool carvedNeighbor = false;

        for (std::size_t index = 0; index < order.size(); index++)
        {
            int dir = order[index];
            GridPos nextNode(currentNode.row + directions[dir][0],
                             currentNode.col + directions[dir][1]);

            if (!isInBounds(nodeRows, nodeCols, nextNode.row, nextNode.col) || visited[toIndex(nodeCols, nextNode.row, nextNode.col)] != 0)
            {
                continue;
            }

            GridPos currentGrid = nodeToGrid(currentNode);
            GridPos nextGrid = nodeToGrid(nextNode);
            GridPos corridor((currentGrid.row + nextGrid.row) / 2,
                             (currentGrid.col + nextGrid.col) / 2);

            grid[toIndex(definition.cols, corridor.row, corridor.col)] = TILE_PATH;
            grid[toIndex(definition.cols, nextGrid.row, nextGrid.col)] = TILE_PATH;

            visited[toIndex(nodeCols, nextNode.row, nextNode.col)] = 1;
            stack.push_back(nextNode);
            carvedNeighbor = true;
            break;
        }

        if (!carvedNeighbor)
        {
            stack.pop_back();
        }
    }
}

void MazeGenerator::buildCriticalPath(const std::vector<TileType> &grid,
                                      const LevelSpec &definition,
                                      GridPos &outExitPos,
                                      std::vector<GridPos> &outPath) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    int nodeRows = (definition.rows - 1) / 2;
    int nodeCols = (definition.cols - 1) / 2;
    GridPos startNode = gridToNode(definition.startPos);
    std::vector<unsigned char> seen(nodeRows * nodeCols, 0);
    std::vector<int> distance(nodeRows * nodeCols, -1);
    std::vector<GridPos> parent(nodeRows * nodeCols, GridPos(-1, -1));
    std::queue<GridPos> frontier;

    frontier.push(startNode);
    seen[toIndex(nodeCols, startNode.row, startNode.col)] = 1;
    distance[toIndex(nodeCols, startNode.row, startNode.col)] = 0;
    parent[toIndex(nodeCols, startNode.row, startNode.col)] = startNode;

    GridPos bestNode = startNode;
    int bestScore = -1000000;

    while (!frontier.empty())
    {
        GridPos currentNode = frontier.front();
        frontier.pop();

        GridPos currentGrid = nodeToGrid(currentNode);
        int degree = 0;

        for (int i = 0; i < 4; i++)
        {
            GridPos nextNode(currentNode.row + directions[i][0],
                             currentNode.col + directions[i][1]);

            if (!isInBounds(nodeRows, nodeCols, nextNode.row, nextNode.col))
            {
                continue;
            }

            GridPos nextGrid = nodeToGrid(nextNode);
            GridPos corridor((currentGrid.row + nextGrid.row) / 2,
                             (currentGrid.col + nextGrid.col) / 2);

            if (!isWalkableValue(grid[toIndex(definition.cols, corridor.row, corridor.col)]))
            {
                continue;
            }

            degree++;

            int nextIndex = toIndex(nodeCols, nextNode.row, nextNode.col);
            if (seen[nextIndex] == 0)
            {
                seen[nextIndex] = 1;
                distance[nextIndex] = distance[toIndex(nodeCols, currentNode.row, currentNode.col)] + 1;
                parent[nextIndex] = currentNode;
                frontier.push(nextNode);
            }
        }

        if (currentNode.row == startNode.row && currentNode.col == startNode.col)
        {
            continue;
        }

        int nodeIndex = toIndex(nodeCols, currentNode.row, currentNode.col);
        int targetDistance = std::abs(currentGrid.row - definition.exitTarget.row) + std::abs(currentGrid.col - definition.exitTarget.col);
        int nodeScore = distance[nodeIndex] * 24 - targetDistance * 3 + currentGrid.col - currentGrid.row;

        if (degree == 1)
        {
            nodeScore += 24;
        }

        if (nodeScore > bestScore)
        {
            bestNode = currentNode;
            bestScore = nodeScore;
        }
    }

    std::vector<GridPos> nodeRoute;
    GridPos currentNode = bestNode;

    while (currentNode.row != startNode.row || currentNode.col != startNode.col)
    {
        nodeRoute.push_back(currentNode);
        currentNode = parent[toIndex(nodeCols, currentNode.row, currentNode.col)];
    }

    nodeRoute.push_back(startNode);
    std::reverse(nodeRoute.begin(), nodeRoute.end());

    outPath.clear();
    if (nodeRoute.empty())
    {
        outExitPos = definition.exitTarget;
        return;
    }

    outPath.push_back(nodeToGrid(nodeRoute[0]));

    for (std::size_t i = 1; i < nodeRoute.size(); i++)
    {
        GridPos previousGrid = nodeToGrid(nodeRoute[i - 1]);
        GridPos currentGrid = nodeToGrid(nodeRoute[i]);

        outPath.push_back(GridPos((previousGrid.row + currentGrid.row) / 2,
                                  (previousGrid.col + currentGrid.col) / 2));
        outPath.push_back(currentGrid);
    }

    outExitPos = nodeToGrid(bestNode);
}

void MazeGenerator::enhanceLayout(std::vector<TileType> &grid,
                                  const LevelSpec &definition,
                                  const std::vector<GridPos> &criticalPath,
                                  const GridPos &exitPos,
                                  unsigned int seed) const
{
    std::vector<unsigned char> criticalMask(definition.rows * definition.cols, 0);
    std::vector<unsigned char> roomMask(definition.rows * definition.cols, 0);
    std::vector<unsigned char> doorwayMask(definition.rows * definition.cols, 0);

    for (std::size_t i = 0; i < criticalPath.size(); i++)
    {
        criticalMask[toIndex(definition.cols, criticalPath[i].row, criticalPath[i].col)] = 1;
    }

    carveRooms(grid,
               definition,
               criticalMask,
               roomMask,
               doorwayMask,
               seed + 101u);

    addLoops(grid,
             definition,
             criticalMask,
             roomMask,
             doorwayMask,
             seed + 211u);

    addDecisionJunctions(grid,
                         definition,
                         criticalPath,
                         exitPos,
                         roomMask,
                         doorwayMask,
                         seed + 389u);
}

int MazeGenerator::carveRooms(std::vector<TileType> &grid,
                              const LevelSpec &definition,
                              const std::vector<unsigned char> &criticalMask,
                              std::vector<unsigned char> &roomMask,
                              std::vector<unsigned char> &doorwayMask,
                              unsigned int seed) const
{
    std::vector<int> distances;
    std::vector<SpatialCandidate> candidates;
    std::mt19937 rng(seed);

    std::vector<GridPos> criticalPath;
    for (int row = 0; row < definition.rows; row++)
    {
        for (int col = 0; col < definition.cols; col++)
        {
            if (criticalMask[toIndex(definition.cols, row, col)] != 0)
            {
                criticalPath.push_back(GridPos(row, col));
            }
        }
    }

    computeDistanceFromCritical(grid,
                                definition.rows,
                                definition.cols,
                                criticalPath,
                                distances);

    for (int row = 1; row < definition.rows - 1; row++)
    {
        for (int col = 1; col < definition.cols - 1; col++)
        {
            int index = toIndex(definition.cols, row, col);
            if (!isWalkableValue(grid[index]) || criticalMask[index] != 0 || (row == definition.startPos.row && col == definition.startPos.col))
            {
                continue;
            }

            SpatialCandidate candidate;
            candidate.pos = GridPos(row, col);
            candidate.distanceFromCritical = distances[index];
            candidate.degree = countWalkableNeighbors(grid, definition.rows, definition.cols, row, col);
            candidate.roomCell = false;

            if (candidate.distanceFromCritical >= 2 && candidate.degree <= 2)
            {
                candidates.push_back(candidate);
            }
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const SpatialCandidate &left, const SpatialCandidate &right)
                     {
                         return left.distanceFromCritical > right.distanceFromCritical;
                     });

    int roomsPlaced = 0;
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (std::size_t i = 0; i < candidates.size() && roomsPlaced < definition.roomBudget; i++)
    {
        GridPos doorway = candidates[i].pos;
        int doorwayIndex = toIndex(definition.cols, doorway.row, doorway.col);
        if (doorwayMask[doorwayIndex] != 0)
        {
            continue;
        }

        std::vector<int> order;
        for (int dir = 0; dir < 4; dir++)
        {
            order.push_back(dir);
        }
        std::shuffle(order.begin(), order.end(), rng);

        for (std::size_t dirIndex = 0; dirIndex < order.size(); dirIndex++)
        {
            int roomRows = definition.roomMinRows + static_cast<int>(rng() % static_cast<unsigned>(definition.roomMaxRows - definition.roomMinRows + 1));
            int roomCols = definition.roomMinCols + static_cast<int>(rng() % static_cast<unsigned>(definition.roomMaxCols - definition.roomMinCols + 1));

            int dir = order[dirIndex];
            if (tryCarveRoom(grid,
                             definition.rows,
                             definition.cols,
                             doorway,
                             directions[dir][0],
                             directions[dir][1],
                             roomRows,
                             roomCols,
                             roomMask,
                             doorwayMask))
            {
                roomsPlaced++;
                break;
            }
        }
    }

    return roomsPlaced;
}

int MazeGenerator::addLoops(std::vector<TileType> &grid,
                            const LevelSpec &definition,
                            const std::vector<unsigned char> &criticalMask,
                            const std::vector<unsigned char> &roomMask,
                            const std::vector<unsigned char> &doorwayMask,
                            unsigned int seed) const
{
    if (definition.loopBudget <= 0)
    {
        return 0;
    }

    struct LoopCandidate
    {
        GridPos pos;
        int pathLength;
        int distanceScore;
    };

    std::vector<GridPos> criticalPath;
    std::vector<int> distances;
    std::vector<LoopCandidate> candidates;
    std::vector<unsigned char> openedMask(definition.rows * definition.cols, 0);
    std::mt19937 rng(seed);
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int row = 0; row < definition.rows; row++)
    {
        for (int col = 0; col < definition.cols; col++)
        {
            if (criticalMask[toIndex(definition.cols, row, col)] != 0)
            {
                criticalPath.push_back(GridPos(row, col));
            }
        }
    }

    computeDistanceFromCritical(grid,
                                definition.rows,
                                definition.cols,
                                criticalPath,
                                distances);

    for (int row = 1; row < definition.rows - 1; row++)
    {
        for (int col = 1; col < definition.cols - 1; col++)
        {
            if (grid[toIndex(definition.cols, row, col)] != TILE_WALL)
            {
                continue;
            }

            std::vector<GridPos> neighbors;
            for (int i = 0; i < 4; i++)
            {
                int nextRow = row + directions[i][0];
                int nextCol = col + directions[i][1];
                int nextIndex = toIndex(definition.cols, nextRow, nextCol);

                if (isWalkableValue(grid[nextIndex]))
                {
                    neighbors.push_back(GridPos(nextRow, nextCol));
                }
            }

            if (neighbors.size() != 2)
            {
                continue;
            }

            int firstIndex = toIndex(definition.cols, neighbors[0].row, neighbors[0].col);
            int secondIndex = toIndex(definition.cols, neighbors[1].row, neighbors[1].col);
            if (criticalMask[firstIndex] != 0 || criticalMask[secondIndex] != 0 || roomMask[firstIndex] != 0 || roomMask[secondIndex] != 0 || doorwayMask[firstIndex] != 0 || doorwayMask[secondIndex] != 0)
            {
                continue;
            }

            if (distances[firstIndex] < 2 || distances[secondIndex] < 2)
            {
                continue;
            }

            int pathLength = shortestPathLength(grid,
                                                definition.rows,
                                                definition.cols,
                                                neighbors[0],
                                                neighbors[1]);
            if (pathLength < 8)
            {
                continue;
            }

            LoopCandidate candidate;
            candidate.pos = GridPos(row, col);
            candidate.pathLength = pathLength;
            candidate.distanceScore = distances[firstIndex] + distances[secondIndex];
            candidates.push_back(candidate);
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const LoopCandidate &left, const LoopCandidate &right)
                     {
                         if (left.pathLength != right.pathLength)
                         {
                             return left.pathLength > right.pathLength;
                         }
                         return left.distanceScore > right.distanceScore;
                     });

    int loopsAdded = 0;
    for (std::size_t i = 0; i < candidates.size() && loopsAdded < definition.loopBudget; i++)
    {
        const GridPos &pos = candidates[i].pos;
        int index = toIndex(definition.cols, pos.row, pos.col);
        if (grid[index] != TILE_WALL)
        {
            continue;
        }

        bool touchesExistingLoop = false;
        for (int rowOffset = -1; rowOffset <= 1 && !touchesExistingLoop; rowOffset++)
        {
            for (int colOffset = -1; colOffset <= 1; colOffset++)
            {
                int checkRow = pos.row + rowOffset;
                int checkCol = pos.col + colOffset;
                if (isInBounds(definition.rows, definition.cols, checkRow, checkCol) && openedMask[toIndex(definition.cols, checkRow, checkCol)] != 0)
                {
                    touchesExistingLoop = true;
                    break;
                }
            }
        }

        if (touchesExistingLoop)
        {
            continue;
        }

        std::vector<GridPos> neighbors;
        for (int dir = 0; dir < 4; dir++)
        {
            int nextRow = pos.row + directions[dir][0];
            int nextCol = pos.col + directions[dir][1];
            int nextIndex = toIndex(definition.cols, nextRow, nextCol);
            if (isWalkableValue(grid[nextIndex]))
            {
                neighbors.push_back(GridPos(nextRow, nextCol));
            }
        }

        if (neighbors.size() != 2)
        {
            continue;
        }

        int pathLength = shortestPathLength(grid,
                                            definition.rows,
                                            definition.cols,
                                            neighbors[0],
                                            neighbors[1]);
        if (pathLength < 8)
        {
            continue;
        }

        grid[index] = TILE_PATH;
        openedMask[index] = 1;
        loopsAdded++;
    }

    return loopsAdded;
}

int MazeGenerator::addDecisionJunctions(std::vector<TileType> &grid,
                                        const LevelSpec &definition,
                                        const std::vector<GridPos> &criticalPath,
                                        const GridPos &exitPos,
                                        const std::vector<unsigned char> &roomMask,
                                        const std::vector<unsigned char> &doorwayMask,
                                        unsigned int seed) const
{
    std::vector<int> distances;
    std::vector<SpatialCandidate> candidates;
    std::vector<unsigned char> usedCenters(definition.rows * definition.cols, 0);
    std::mt19937 rng(seed);
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    computeDistanceFromCritical(grid,
                                definition.rows,
                                definition.cols,
                                criticalPath,
                                distances);

    for (int row = 1; row < definition.rows - 1; row++)
    {
        for (int col = 1; col < definition.cols - 1; col++)
        {
            int index = toIndex(definition.cols, row, col);
            if (!isNodeCell(GridPos(row, col)) || !isWalkableValue(grid[index]) || roomMask[index] != 0 || doorwayMask[index] != 0)
            {
                continue;
            }

            if ((row == definition.startPos.row && col == definition.startPos.col) || (row == exitPos.row && col == exitPos.col))
            {
                continue;
            }

            SpatialCandidate candidate;
            candidate.pos = GridPos(row, col);
            candidate.distanceFromCritical = distances[index];
            candidate.degree = countWalkableNeighbors(grid, definition.rows, definition.cols, row, col);
            candidate.roomCell = false;

            if (candidate.degree < 2 || candidate.degree > 3 || candidate.distanceFromCritical == -1)
            {
                continue;
            }

            candidates.push_back(candidate);
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const SpatialCandidate &left, const SpatialCandidate &right)
                     {
                         int leftScore = (left.degree == 2 ? 120 : 0) - left.distanceFromCritical * 10;
                         int rightScore = (right.degree == 2 ? 120 : 0) - right.distanceFromCritical * 10;
                         return leftScore > rightScore;
                     });

    int junctionsAdded = 0;
    for (std::size_t i = 0; i < candidates.size() && junctionsAdded < definition.junctionTarget; i++)
    {
        const GridPos &center = candidates[i].pos;
        int centerIndex = toIndex(definition.cols, center.row, center.col);
        bool tooCloseToExisting = false;

        for (int rowOffset = -5; rowOffset <= 5 && !tooCloseToExisting; rowOffset++)
        {
            for (int colOffset = -5; colOffset <= 5; colOffset++)
            {
                int checkRow = center.row + rowOffset;
                int checkCol = center.col + colOffset;
                if (!isInBounds(definition.rows, definition.cols, checkRow, checkCol))
                {
                    continue;
                }

                if (usedCenters[toIndex(definition.cols, checkRow, checkCol)] != 0 && std::abs(rowOffset) + std::abs(colOffset) < 6)
                {
                    tooCloseToExisting = true;
                    break;
                }
            }
        }

        if (tooCloseToExisting)
        {
            continue;
        }

        struct BranchDirection
        {
            int rowStep;
            int colStep;
        };

        std::vector<BranchDirection> options;
        for (int dir = 0; dir < 4; dir++)
        {
            int corridorRow = center.row + directions[dir][0];
            int corridorCol = center.col + directions[dir][1];
            int nodeRow = center.row + directions[dir][0] * 2;
            int nodeCol = center.col + directions[dir][1] * 2;

            if (!isInBounds(definition.rows, definition.cols, corridorRow, corridorCol) || !isInBounds(definition.rows, definition.cols, nodeRow, nodeCol) || grid[toIndex(definition.cols, corridorRow, corridorCol)] != TILE_WALL || grid[toIndex(definition.cols, nodeRow, nodeCol)] != TILE_WALL)
            {
                continue;
            }

            bool isolated = true;
            for (int checkDir = 0; checkDir < 4; checkDir++)
            {
                int nextRow = nodeRow + directions[checkDir][0];
                int nextCol = nodeCol + directions[checkDir][1];

                if (nextRow == corridorRow && nextCol == corridorCol)
                {
                    continue;
                }

                if (isInBounds(definition.rows, definition.cols, nextRow, nextCol) && isWalkableValue(grid[toIndex(definition.cols, nextRow, nextCol)]))
                {
                    isolated = false;
                    break;
                }
            }

            if (isolated)
            {
                BranchDirection option = {directions[dir][0], directions[dir][1]};
                options.push_back(option);
            }
        }

        if (options.empty())
        {
            continue;
        }

        std::shuffle(options.begin(), options.end(), rng);
        BranchDirection firstBranch = options[0];
        GridPos previous = center;
        GridPos current(center.row + firstBranch.rowStep * 2,
                        center.col + firstBranch.colStep * 2);

        grid[toIndex(definition.cols, center.row + firstBranch.rowStep,
                     center.col + firstBranch.colStep)] = TILE_PATH;
        grid[toIndex(definition.cols, current.row, current.col)] = TILE_PATH;

        int extensionBudget = 1 + static_cast<int>(rng() % static_cast<unsigned>(definition.loopBudget > 0 ? 3 : 2));
        int previousRowStep = firstBranch.rowStep;
        int previousColStep = firstBranch.colStep;

        for (int step = 0; step < extensionBudget; step++)
        {
            std::vector<BranchDirection> extensionOptions;
            for (int dir = 0; dir < 4; dir++)
            {
                int corridorRow = current.row + directions[dir][0];
                int corridorCol = current.col + directions[dir][1];
                int nodeRow = current.row + directions[dir][0] * 2;
                int nodeCol = current.col + directions[dir][1] * 2;

                if (nodeRow == previous.row && nodeCol == previous.col)
                {
                    continue;
                }

                if (!isInBounds(definition.rows, definition.cols, corridorRow, corridorCol) || !isInBounds(definition.rows, definition.cols, nodeRow, nodeCol) || grid[toIndex(definition.cols, corridorRow, corridorCol)] != TILE_WALL || grid[toIndex(definition.cols, nodeRow, nodeCol)] != TILE_WALL)
                {
                    continue;
                }

                bool isolated = true;
                for (int checkDir = 0; checkDir < 4; checkDir++)
                {
                    int nextRow = nodeRow + directions[checkDir][0];
                    int nextCol = nodeCol + directions[checkDir][1];

                    if (nextRow == corridorRow && nextCol == corridorCol)
                    {
                        continue;
                    }

                    if (isInBounds(definition.rows, definition.cols, nextRow, nextCol) && isWalkableValue(grid[toIndex(definition.cols, nextRow, nextCol)]))
                    {
                        isolated = false;
                        break;
                    }
                }

                if (isolated)
                {
                    BranchDirection option = {directions[dir][0], directions[dir][1]};
                    extensionOptions.push_back(option);
                }
            }

            if (extensionOptions.empty())
            {
                break;
            }

            std::shuffle(extensionOptions.begin(), extensionOptions.end(), rng);
            std::stable_sort(extensionOptions.begin(), extensionOptions.end(),
                             [previousRowStep, previousColStep](const BranchDirection &left, const BranchDirection &right)
                             {
                                 bool leftTurns = left.rowStep != previousRowStep || left.colStep != previousColStep;
                                 bool rightTurns = right.rowStep != previousRowStep || right.colStep != previousColStep;
                                 return leftTurns && !rightTurns;
                             });

            BranchDirection nextBranch = extensionOptions[0];
            GridPos nextNode(current.row + nextBranch.rowStep * 2,
                             current.col + nextBranch.colStep * 2);

            grid[toIndex(definition.cols, current.row + nextBranch.rowStep,
                         current.col + nextBranch.colStep)] = TILE_PATH;
            grid[toIndex(definition.cols, nextNode.row, nextNode.col)] = TILE_PATH;

            previous = current;
            current = nextNode;
            previousRowStep = nextBranch.rowStep;
            previousColStep = nextBranch.colStep;
        }

        usedCenters[centerIndex] = 1;
        junctionsAdded++;
    }

    return junctionsAdded;
}

bool MazeGenerator::tryCarveRoom(std::vector<TileType> &grid,
                                 int rows,
                                 int cols,
                                 const GridPos &doorway,
                                 int directionRow,
                                 int directionCol,
                                 int roomRows,
                                 int roomCols,
                                 std::vector<unsigned char> &roomMask,
                                 std::vector<unsigned char> &doorwayMask) const
{
    int top = 0;
    int left = 0;
    int bottom = 0;
    int right = 0;
    int entryRow = 0;
    int entryCol = 0;

    if (directionRow == -1)
    {
        bottom = doorway.row - 1;
        top = bottom - roomRows + 1;
        left = doorway.col - roomCols / 2;
        right = left + roomCols - 1;
        entryRow = bottom;
        entryCol = doorway.col;
    }
    else if (directionRow == 1)
    {
        top = doorway.row + 1;
        bottom = top + roomRows - 1;
        left = doorway.col - roomCols / 2;
        right = left + roomCols - 1;
        entryRow = top;
        entryCol = doorway.col;
    }
    else if (directionCol == -1)
    {
        right = doorway.col - 1;
        left = right - roomCols + 1;
        top = doorway.row - roomRows / 2;
        bottom = top + roomRows - 1;
        entryRow = doorway.row;
        entryCol = right;
    }
    else
    {
        left = doorway.col + 1;
        right = left + roomCols - 1;
        top = doorway.row - roomRows / 2;
        bottom = top + roomRows - 1;
        entryRow = doorway.row;
        entryCol = left;
    }

    if (top < 1 || left < 1 || bottom > rows - 2 || right > cols - 2)
    {
        return false;
    }

    for (int row = top; row <= bottom; row++)
    {
        for (int col = left; col <= right; col++)
        {
            if (grid[toIndex(cols, row, col)] != TILE_WALL)
            {
                return false;
            }
        }
    }

    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int row = top; row <= bottom; row++)
    {
        for (int col = left; col <= right; col++)
        {
            for (int i = 0; i < 4; i++)
            {
                int nextRow = row + directions[i][0];
                int nextCol = col + directions[i][1];

                if (nextRow >= top && nextRow <= bottom && nextCol >= left && nextCol <= right)
                {
                    continue;
                }

                if (nextRow == doorway.row && nextCol == doorway.col && row == entryRow && col == entryCol)
                {
                    continue;
                }

                if (isInBounds(rows, cols, nextRow, nextCol) && isWalkableValue(grid[toIndex(cols, nextRow, nextCol)]))
                {
                    return false;
                }
            }
        }
    }

    for (int row = top; row <= bottom; row++)
    {
        for (int col = left; col <= right; col++)
        {
            grid[toIndex(cols, row, col)] = TILE_PATH;
            roomMask[toIndex(cols, row, col)] = 1;
        }
    }

    doorwayMask[toIndex(cols, doorway.row, doorway.col)] = 1;
    return true;
}

GridPos MazeGenerator::gridToNode(const GridPos &gridPos) const
{
    return GridPos((gridPos.row - 1) / 2, (gridPos.col - 1) / 2);
}

GridPos MazeGenerator::nodeToGrid(const GridPos &nodePos) const
{
    return GridPos(nodePos.row * 2 + 1, nodePos.col * 2 + 1);
}
