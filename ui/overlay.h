#ifndef OVERLAY_H
#define OVERLAY_H

#include "../core/gamestate.h"
#include "../gameplay/level.h"
#include "../data/highscore.h"

namespace Overlay
{
    void drawLevelClearOverlay(int windowWidth, int windowHeight,
                               const GameStateData& gameState,
                               const Level& level);
    void drawEndOverlay(int windowWidth, int windowHeight,
                       const char* title, const char* subtitle,
                       const GameStateData& gameState,
                       const HighScoreManager& highScores);
}

#endif // OVERLAY_H
