#include "generator.h"
#include <algorithm>
#include <cstring>
#include <queue>
#include <random>

MazeGenerator::MazeGenerator()
{
    std::memset(visited, 0, sizeof(visited));
}

void MazeGenerator::generate(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                             GridPos startPos,
                             const LevelDefinition& definition,
                             GridPos& outExitPos,
                             std::vector<GridPos>& outPath)
{
    MazeCandidate bestCandidate;
    bool hasBestCandidate = false;
    bool hasQualifiedCandidate = false;

    for (int attempt = 0; attempt < definition.candidateCount; attempt++)
    {
        MazeCandidate candidate;
        generateLayout(candidate, startPos, definition.seed + static_cast<unsigned int>(attempt * 977));

        bool meetsSpec = candidate.solutionLength >= definition.minSolutionLength
                      && candidate.deadEndCount >= definition.minDeadEnds;

        if (!hasBestCandidate
            || (meetsSpec && !hasQualifiedCandidate)
            || (meetsSpec == hasQualifiedCandidate && candidate.score > bestCandidate.score))
        {
            bestCandidate = candidate;
            hasBestCandidate = true;
            hasQualifiedCandidate = meetsSpec;
        }
    }

    std::memcpy(grid, bestCandidate.grid, sizeof(bestCandidate.grid));
    outExitPos = bestCandidate.exitPos;
    outPath = bestCandidate.solutionPath;

    applyObjectivesAndHazards(grid, outPath, outExitPos, definition, definition.seed + 7919u);
}

void MazeGenerator::initializeGrid(int grid[Config::MAZE_ROWS][Config::MAZE_COLS])
{
    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            grid[row][col] = TILE_WALL;
        }
    }
}

void MazeGenerator::generateLayout(MazeCandidate& outCandidate,
                                   const GridPos& startPos,
                                   unsigned int seed)
{
    initializeGrid(outCandidate.grid);
    carvePerfectMaze(outCandidate.grid, gridToNode(startPos), seed);
    buildSolutionPath(outCandidate.grid,
                      gridToNode(startPos),
                      outCandidate.exitPos,
                      outCandidate.solutionPath);

    outCandidate.solutionLength = static_cast<int>(outCandidate.solutionPath.size());
    outCandidate.deadEndCount = countDeadEnds(outCandidate.grid, outCandidate.solutionPath);
    outCandidate.score = outCandidate.solutionLength * 9 + outCandidate.deadEndCount * 7;
}

void MazeGenerator::carvePerfectMaze(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                     const GridPos& startNode,
                                     unsigned int seed)
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    std::memset(visited, 0, sizeof(visited));

    std::mt19937 rng(seed);
    std::vector<GridPos> stack;
    stack.push_back(startNode);
    visited[startNode.row][startNode.col] = true;

    GridPos startGrid = nodeToGrid(startNode);
    grid[startGrid.row][startGrid.col] = TILE_PATH;

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

        for (size_t index = 0; index < order.size(); index++)
        {
            int dir = order[index];
            GridPos nextNode(currentNode.row + directions[dir][0],
                             currentNode.col + directions[dir][1]);

            if (!isNodeValid(nextNode.row, nextNode.col) || visited[nextNode.row][nextNode.col])
            {
                continue;
            }

            GridPos currentGrid = nodeToGrid(currentNode);
            GridPos nextGrid = nodeToGrid(nextNode);
            GridPos corridor((currentGrid.row + nextGrid.row) / 2,
                             (currentGrid.col + nextGrid.col) / 2);

            grid[corridor.row][corridor.col] = TILE_PATH;
            grid[nextGrid.row][nextGrid.col] = TILE_PATH;

            visited[nextNode.row][nextNode.col] = true;
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

void MazeGenerator::buildSolutionPath(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                      const GridPos& startNode,
                                      GridPos& outExitPos,
                                      std::vector<GridPos>& outPath) const
{
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool seen[NODE_ROWS][NODE_COLS];
    int distance[NODE_ROWS][NODE_COLS];
    GridPos parent[NODE_ROWS][NODE_COLS];

    std::memset(seen, 0, sizeof(seen));
    std::memset(distance, 0, sizeof(distance));

    std::queue<GridPos> frontier;
    frontier.push(startNode);
    seen[startNode.row][startNode.col] = true;
    parent[startNode.row][startNode.col] = startNode;
    GridPos exitNode = startNode;

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

            if (!isNodeValid(nextNode.row, nextNode.col))
            {
                continue;
            }

            GridPos nextGrid = nodeToGrid(nextNode);
            GridPos corridor((currentGrid.row + nextGrid.row) / 2,
                             (currentGrid.col + nextGrid.col) / 2);

            if (grid[corridor.row][corridor.col] == TILE_WALL)
            {
                continue;
            }

            degree++;

            if (!seen[nextNode.row][nextNode.col])
            {
                seen[nextNode.row][nextNode.col] = true;
                parent[nextNode.row][nextNode.col] = currentNode;
                distance[nextNode.row][nextNode.col] = distance[currentNode.row][currentNode.col] + 1;
                frontier.push(nextNode);
            }
        }

        if ((currentNode.row != startNode.row || currentNode.col != startNode.col)
            && degree == 1
            && distance[currentNode.row][currentNode.col] >= distance[exitNode.row][exitNode.col])
        {
            exitNode = currentNode;
        }
    }

    std::vector<GridPos> nodeRoute;
    GridPos currentNode = exitNode;

    while (currentNode.row != startNode.row || currentNode.col != startNode.col)
    {
        nodeRoute.push_back(currentNode);
        currentNode = parent[currentNode.row][currentNode.col];
    }

    nodeRoute.push_back(startNode);
    std::reverse(nodeRoute.begin(), nodeRoute.end());

    outPath.clear();
    if (nodeRoute.empty())
    {
        return;
    }

    outPath.push_back(nodeToGrid(nodeRoute[0]));

    for (size_t i = 1; i < nodeRoute.size(); i++)
    {
        GridPos previousGrid = nodeToGrid(nodeRoute[i - 1]);
        GridPos currentGrid = nodeToGrid(nodeRoute[i]);

        outPath.push_back(GridPos((previousGrid.row + currentGrid.row) / 2,
                                  (previousGrid.col + currentGrid.col) / 2));
        outPath.push_back(currentGrid);
    }

    outExitPos = nodeToGrid(exitNode);
}

void MazeGenerator::applyObjectivesAndHazards(int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                              const std::vector<GridPos>& solutionPath,
                                              const GridPos& exitPos,
                                              const LevelDefinition& definition,
                                              unsigned int seed) const
{
    bool protectedCells[Config::MAZE_ROWS][Config::MAZE_COLS];
    bool reservedCells[Config::MAZE_ROWS][Config::MAZE_COLS];
    int distanceFromSolution[Config::MAZE_ROWS][Config::MAZE_COLS];
    std::vector<CellCandidate> deadEnds;
    std::vector<CellCandidate> branches;
    std::mt19937 rng(seed);

    std::memset(protectedCells, 0, sizeof(protectedCells));
    std::memset(reservedCells, 0, sizeof(reservedCells));

    for (size_t i = 0; i < solutionPath.size(); i++)
    {
        protectedCells[solutionPath[i].row][solutionPath[i].col] = true;
        reservedCells[solutionPath[i].row][solutionPath[i].col] = true;
    }

    GridPos keyCell = chooseKeyCell(solutionPath, definition.keyProgressPercent);
    grid[exitPos.row][exitPos.col] = TILE_LOCKED_EXIT;
    grid[keyCell.row][keyCell.col] = TILE_KEY;

    computeDistanceFromSolution(grid, solutionPath, distanceFromSolution);

    reservedCells[exitPos.row][exitPos.col] = true;
    reservedCells[keyCell.row][keyCell.col] = true;
    reservedCells[Config::START_ROW][Config::START_COL] = true;

    for (int row = 1; row < Config::MAZE_ROWS - 1; row++)
    {
        for (int col = 1; col < Config::MAZE_COLS - 1; col++)
        {
            if (protectedCells[row][col] || grid[row][col] != TILE_PATH)
            {
                continue;
            }

            int neighbors = countWalkableNeighbors(grid, row, col);
            CellCandidate candidate = {GridPos(row, col), distanceFromSolution[row][col]};

            if (neighbors <= 1)
            {
                deadEnds.push_back(candidate);
            }
            else
            {
                branches.push_back(candidate);
            }
        }
    }

    std::shuffle(deadEnds.begin(), deadEnds.end(), rng);
    std::shuffle(branches.begin(), branches.end(), rng);

    std::stable_sort(deadEnds.begin(), deadEnds.end(),
        [](const CellCandidate& left, const CellCandidate& right)
        {
            return left.distanceFromSolution > right.distanceFromSolution;
        });

    std::stable_sort(branches.begin(), branches.end(),
        [](const CellCandidate& left, const CellCandidate& right)
        {
            return left.distanceFromSolution > right.distanceFromSolution;
        });

    int placedTraps = 0;

    for (size_t i = 0; i < deadEnds.size() && placedTraps < definition.trapCount; i++)
    {
        const GridPos& pos = deadEnds[i].pos;
        if (reservedCells[pos.row][pos.col])
        {
            continue;
        }

        grid[pos.row][pos.col] = TILE_TRAP;
        reservedCells[pos.row][pos.col] = true;
        placedTraps++;
    }

    for (size_t i = 0; i < branches.size() && placedTraps < definition.trapCount; i++)
    {
        const GridPos& pos = branches[i].pos;
        if (reservedCells[pos.row][pos.col])
        {
            continue;
        }

        grid[pos.row][pos.col] = TILE_TRAP;
        reservedCells[pos.row][pos.col] = true;
        placedTraps++;
    }

    std::stable_sort(deadEnds.begin(), deadEnds.end(),
        [](const CellCandidate& left, const CellCandidate& right)
        {
            return left.distanceFromSolution < right.distanceFromSolution;
        });

    std::stable_sort(branches.begin(), branches.end(),
        [](const CellCandidate& left, const CellCandidate& right)
        {
            return left.distanceFromSolution < right.distanceFromSolution;
        });

    int placedObstacles = 0;

    for (size_t i = 0; i < deadEnds.size() && placedObstacles < definition.obstacleCount; i++)
    {
        const GridPos& pos = deadEnds[i].pos;
        if (reservedCells[pos.row][pos.col])
        {
            continue;
        }

        grid[pos.row][pos.col] = TILE_OBSTACLE;
        reservedCells[pos.row][pos.col] = true;
        placedObstacles++;
    }

    for (size_t i = 0; i < branches.size() && placedObstacles < definition.obstacleCount; i++)
    {
        const GridPos& pos = branches[i].pos;
        if (reservedCells[pos.row][pos.col])
        {
            continue;
        }

        grid[pos.row][pos.col] = TILE_OBSTACLE;
        reservedCells[pos.row][pos.col] = true;
        placedObstacles++;
    }
}

void MazeGenerator::computeDistanceFromSolution(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                                const std::vector<GridPos>& solutionPath,
                                                int distances[Config::MAZE_ROWS][Config::MAZE_COLS]) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    std::queue<GridPos> frontier;

    for (int row = 0; row < Config::MAZE_ROWS; row++)
    {
        for (int col = 0; col < Config::MAZE_COLS; col++)
        {
            distances[row][col] = -1;
        }
    }

    for (size_t i = 0; i < solutionPath.size(); i++)
    {
        distances[solutionPath[i].row][solutionPath[i].col] = 0;
        frontier.push(solutionPath[i]);
    }

    while (!frontier.empty())
    {
        GridPos current = frontier.front();
        frontier.pop();

        for (int i = 0; i < 4; i++)
        {
            int nextRow = current.row + directions[i][0];
            int nextCol = current.col + directions[i][1];

            if (nextRow < 0 || nextRow >= Config::MAZE_ROWS
                || nextCol < 0 || nextCol >= Config::MAZE_COLS
                || distances[nextRow][nextCol] != -1
                || !isWalkableValue(grid[nextRow][nextCol]))
            {
                continue;
            }

            distances[nextRow][nextCol] = distances[current.row][current.col] + 1;
            frontier.push(GridPos(nextRow, nextCol));
        }
    }
}

GridPos MazeGenerator::chooseKeyCell(const std::vector<GridPos>& solutionPath,
                                     int keyProgressPercent) const
{
    std::vector<GridPos> nodeCells;

    for (size_t i = 0; i < solutionPath.size(); i++)
    {
        const GridPos& cell = solutionPath[i];
        if (cell.row % 2 == 1 && cell.col % 2 == 1)
        {
            nodeCells.push_back(cell);
        }
    }

    if (nodeCells.size() <= 2)
    {
        return solutionPath[solutionPath.size() / 2];
    }

    int index = static_cast<int>((nodeCells.size() - 1) * keyProgressPercent / 100);
    if (index < 1)
    {
        index = 1;
    }
    if (index > static_cast<int>(nodeCells.size()) - 2)
    {
        index = static_cast<int>(nodeCells.size()) - 2;
    }

    return nodeCells[index];
}

int MazeGenerator::countDeadEnds(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                 const std::vector<GridPos>& solutionPath) const
{
    bool protectedCells[Config::MAZE_ROWS][Config::MAZE_COLS];
    int deadEnds = 0;

    std::memset(protectedCells, 0, sizeof(protectedCells));

    for (size_t i = 0; i < solutionPath.size(); i++)
    {
        protectedCells[solutionPath[i].row][solutionPath[i].col] = true;
    }

    for (int row = 1; row < Config::MAZE_ROWS - 1; row++)
    {
        for (int col = 1; col < Config::MAZE_COLS - 1; col++)
        {
            if (protectedCells[row][col] || !isWalkableValue(grid[row][col]))
            {
                continue;
            }

            if (countWalkableNeighbors(grid, row, col) <= 1)
            {
                deadEnds++;
            }
        }
    }

    return deadEnds;
}

int MazeGenerator::countWalkableNeighbors(const int grid[Config::MAZE_ROWS][Config::MAZE_COLS],
                                          int row,
                                          int col) const
{
    static const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int neighbors = 0;

    for (int i = 0; i < 4; i++)
    {
        int nextRow = row + directions[i][0];
        int nextCol = col + directions[i][1];

        if (nextRow >= 0 && nextRow < Config::MAZE_ROWS
            && nextCol >= 0 && nextCol < Config::MAZE_COLS
            && isWalkableValue(grid[nextRow][nextCol]))
        {
            neighbors++;
        }
    }

    return neighbors;
}

bool MazeGenerator::isNodeValid(int row, int col) const
{
    return row >= 0 && row < NODE_ROWS && col >= 0 && col < NODE_COLS;
}

bool MazeGenerator::isWalkableValue(int tileValue) const
{
    return tileValue != TILE_WALL && tileValue != TILE_OBSTACLE;
}

GridPos MazeGenerator::gridToNode(const GridPos& gridPos) const
{
    return GridPos((gridPos.row - 1) / 2, (gridPos.col - 1) / 2);
}

GridPos MazeGenerator::nodeToGrid(const GridPos& nodePos) const
{
    return GridPos(nodePos.row * 2 + 1, nodePos.col * 2 + 1);
}
