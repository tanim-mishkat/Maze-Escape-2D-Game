#ifndef PLAYER_H
#define PLAYER_H

#include "../core/types.h"

class Player
{
public:
    Player();

    void reset(const GridPos& startPos);
    void setPosition(const GridPos& grid);
    void setPosition(int row, int col);

    GridPos getGridPos() const { return pos; }

    // Movement flags
    bool isMoveUp() const { return moveUp; }
    bool isMoveDown() const { return moveDown; }
    bool isMoveLeft() const { return moveLeft; }
    bool isMoveRight() const { return moveRight; }

    void setMoveUp(bool value) { moveUp = value; }
    void setMoveDown(bool value) { moveDown = value; }
    void setMoveLeft(bool value) { moveLeft = value; }
    void setMoveRight(bool value) { moveRight = value; }

    void resetMovementFlags();

private:
    GridPos pos;
    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;
};

#endif // PLAYER_H
