#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "types.h"
#include "config.h"

// Global game state container
struct GameStateData
{
    GameState state;
    TitleStep titleStep;
    int currentLevelIndex;
    int selectedStartLevelIndex;
    int menuSelection;
    int lives;
    int score;
    int levelClearBonus;
    bool scoreSaved;
    bool hasKey;
    bool soundEnabled;
    char playerName[Config::MAX_PLAYER_NAME_LENGTH + 1];

    GameStateData();
    void reset();
    bool isPlaying() const;
};

#endif // GAMESTATE_H
