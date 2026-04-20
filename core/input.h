#ifndef INPUT_H
#define INPUT_H

// Input state manager for handling keyboard input tracking and movement repeat logic
namespace InputManager
{
    enum MoveDirection
    {
        MOVE_DIRECTION_NONE = -1,
        MOVE_DIRECTION_UP = 0,
        MOVE_DIRECTION_DOWN = 1,
        MOVE_DIRECTION_LEFT = 2,
        MOVE_DIRECTION_RIGHT = 3
    };

    // State tracking for input handling
    struct State
    {
        int movePressSequence;               // Counter for tracking which key was pressed most recently
        int movePressOrder[4];               // Press order for each direction (0=not held, >0=press order)
        MoveDirection repeatedMoveDirection; // Currently repeating direction (for held key)
        int nextRepeatedMoveMs;              // Time (in ms) when next repeat move should occur

        State()
            : movePressSequence(0),
              repeatedMoveDirection(MOVE_DIRECTION_NONE),
              nextRepeatedMoveMs(0)
        {
            for (int i = 0; i < 4; i++)
            {
                movePressOrder[i] = 0;
            }
        }

        void reset()
        {
            movePressSequence = 0;
            repeatedMoveDirection = MOVE_DIRECTION_NONE;
            nextRepeatedMoveMs = 0;
            for (int i = 0; i < 4; i++)
            {
                movePressOrder[i] = 0;
            }
        }
    };

    // Convert MoveDirection to row/col delta
    void getMovementDelta(MoveDirection direction, int &rowDelta, int &colDelta);

    // Get the next repeat time delay (initial vs repeat)
    int getRepeatDelayMs(bool isInitialPress);
}

#endif // INPUT_H
