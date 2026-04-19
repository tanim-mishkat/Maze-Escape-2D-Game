#include "player.h"
#include "../utils/coords.h"

Player::Player()
    : pos(0.0f, 0.0f), moveUp(false), moveDown(false), moveLeft(false), moveRight(false)
{
    reset();
}

void Player::reset()
{
    setPosition(Config::START_ROW, Config::START_COL);
    resetMovementFlags();
}

void Player::setPosition(const GridPos& grid)
{
    setPosition(grid.row, grid.col);
}

void Player::setPosition(int row, int col)
{
    pos = Coords::playerGridToScreen(row, col, Config::PLAYER_PADDING);
}

GridPos Player::getGridPos() const
{
    return Coords::playerScreenToGrid(pos.x, pos.y, Config::PLAYER_SIZE);
}

void Player::resetMovementFlags()
{
    moveUp = false;
    moveDown = false;
    moveLeft = false;
    moveRight = false;
}
