#include "gamestate.h"
#include <cstring>
#include <cstdio>

GameStateData::GameStateData()
    : state(STATE_MAIN_MENU),
      titleStep(TITLE_STEP_WELCOME),
      currentLevelIndex(0),
      selectedStartLevelIndex(0),
      menuSelection(0),
      lives(Config::MAX_LIVES),
      score(0),
      lastLevelScore(0),
      lastLevelTimeMs(0),
      scoreSaved(false),
      settingsEditingName(false),
      mouseX(0),
      mouseY(0)
{
    std::snprintf(playerName, sizeof(playerName), "PLAYER");
    std::snprintf(settingsDraftName, sizeof(settingsDraftName), "%s", playerName);
}

void GameStateData::reset()
{
    state = STATE_MAIN_MENU;
    titleStep = TITLE_STEP_WELCOME;
    currentLevelIndex = 0;
    selectedStartLevelIndex = 0;
    menuSelection = 0;
    lives = Config::MAX_LIVES;
    score = 0;
    lastLevelScore = 0;
    lastLevelTimeMs = 0;
    scoreSaved = false;
    settingsEditingName = false;
    std::snprintf(settingsDraftName, sizeof(settingsDraftName), "%s", playerName);
}

bool GameStateData::isPlaying() const
{
    return state == STATE_PLAYING;
}
