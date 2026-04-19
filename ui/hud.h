#ifndef HUD_H
#define HUD_H

#include "../core/gamestate.h"
#include "../gameplay/level.h"
#include "../data/highscore.h"
#include "../utils/timer.h"

namespace HUD
{
    void drawPanel(float x, float y, float width, float height,
                  float accentR, float accentG, float accentB, float alpha = 0.84f);
    void drawInfoCard(float x, float y, float width, float height,
                     const char* label, const char* value,
                     float accentR, float accentG, float accentB);
    void drawHud(int windowWidth, int windowHeight, const GameStateData& gameState,
                const Level& level, const Timer& timer, const HighScoreManager& highScores);
}

#endif // HUD_H
