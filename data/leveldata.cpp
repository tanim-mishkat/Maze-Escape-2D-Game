#include "leveldata.h"

namespace LevelData
{
    const LevelSpec levels[Config::TOTAL_LEVELS] =
    {
        {
            "Orientation Grid",
            15, 21,
            GridPos(13, 1),
            GridPos(1, 19),
            1327u,
            28,
            42,
            10,
            1,
            3, 4,
            3, 4,
            0,
            4,
            35000
        },
        {
            "Archive Drift",
            17, 25,
            GridPos(15, 1),
            GridPos(1, 23),
            2441u,
            34,
            58,
            14,
            2,
            3, 4,
            3, 4,
            0,
            4,
            42000
        },
        {
            "Foundry Knots",
            19, 29,
            GridPos(17, 1),
            GridPos(1, 27),
            3761u,
            40,
            76,
            18,
            3,
            3, 5,
            3, 4,
            0,
            4,
            50000
        },
        {
            "Blackout Loop",
            25, 35,
            GridPos(23, 1),
            GridPos(1, 33),
            4987u,
            52,
            110,
            24,
            4,
            4, 6,
            4, 6,
            2,
            5,
            68000
        },
        {
            "Final Nexus",
            29, 41,
            GridPos(27, 1),
            GridPos(1, 39),
            6421u,
            64,
            145,
            30,
            6,
            4, 6,
            4, 6,
            4,
            6,
            82000
        }
    };
}
