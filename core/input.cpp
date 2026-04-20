#include "input.h"
#include "config.h"

namespace InputManager
{
    void getMovementDelta(MoveDirection direction, int &rowDelta, int &colDelta)
    {
        rowDelta = 0;
        colDelta = 0;

        switch (direction)
        {
        case MOVE_DIRECTION_UP:
            rowDelta = -1;
            break;
        case MOVE_DIRECTION_DOWN:
            rowDelta = 1;
            break;
        case MOVE_DIRECTION_LEFT:
            colDelta = -1;
            break;
        case MOVE_DIRECTION_RIGHT:
            colDelta = 1;
            break;
        default:
            break;
        }
    }

    int getRepeatDelayMs(bool isInitialPress)
    {
        return isInitialPress
                   ? Config::HOLD_MOVE_INITIAL_DELAY_MS
                   : Config::HOLD_MOVE_REPEAT_INTERVAL_MS;
    }
}
