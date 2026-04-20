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
    char playerName[Config::MAX_PLAYER_NAME_LENGTH + 1];
    char settingsDraftName[Config::MAX_PLAYER_NAME_LENGTH + 1];
    bool settingsEditingName;
    int mouseX, mouseY;  // current mouse position in OpenGL coords (y=0 at bottom)

    GameStateData();
    void reset();
    bool isPlaying() const;
};

#endif // GAMESTATE_H
