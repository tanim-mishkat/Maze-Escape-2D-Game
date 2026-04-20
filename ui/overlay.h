#ifndef OVERLAY_H
#define OVERLAY_H

#include "../core/gamestate.h"
#include "../gameplay/level.h"
#include "../data/highscore.h"

namespace Overlay
{
    enum EndOverlayAction
    {
        END_OVERLAY_ACTION_NONE = -1,
        END_OVERLAY_ACTION_REPLAY = 0,
        END_OVERLAY_ACTION_MENU = 1,
        END_OVERLAY_ACTION_QUIT = 2
    };

    RectF getLevelClearButtonRect(int windowWidth, int windowHeight);
    bool isLevelClearContinueHit(int windowWidth, int windowHeight, int mouseX, int mouseY);
    RectF getEndOverlayButtonRect(int windowWidth, int windowHeight, EndOverlayAction action);
    EndOverlayAction hitTestEndOverlay(int windowWidth, int windowHeight, int mouseX, int mouseY);

    void drawLevelClearOverlay(int windowWidth, int windowHeight,
                               const GameStateData& gameState,
                               const Level& level);
    void drawEndOverlay(int windowWidth, int windowHeight,
                        const char* title, const char* subtitle,
                        const GameStateData& gameState,
                        const HighScoreManager& highScores);
}

#endif // OVERLAY_H
