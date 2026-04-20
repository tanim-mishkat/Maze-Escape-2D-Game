#include "player.h"

Player::Player()
    : pos(0, 0), moveUp(false), moveDown(false), moveLeft(false), moveRight(false)
{
}

void Player::reset(const GridPos& startPos)
{
    setPosition(startPos);
    resetMovementFlags();
}

void Player::setPosition(const GridPos& grid)
{
    setPosition(grid.row, grid.col);
}

void Player::setPosition(int row, int col)
{
    pos = GridPos(row, col);
}

void Player::resetMovementFlags()
{
    moveUp = false;
    moveDown = false;
    moveLeft = false;
    moveRight = false;
}
