#ifndef MENU_H
#define MENU_H

#include "../core/gamestate.h"
#include "../data/highscore.h"

namespace Menu
{
    void drawMainMenu(int windowWidth, int windowHeight,
                     const GameStateData& gameState,
                     const HighScoreManager& highScores);
    void drawPauseMenu(int windowWidth, int windowHeight);
    void drawHowToPlay(int windowWidth, int windowHeight);
    void drawSettings(int windowWidth, int windowHeight,
                     const GameStateData& gameState);
}

#endif // MENU_H
