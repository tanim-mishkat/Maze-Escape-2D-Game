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
      levelClearBonus(0),
      scoreSaved(false),
      hasKey(false),
      soundEnabled(true),
      mouseX(0),
      mouseY(0)
{
    std::snprintf(playerName, sizeof(playerName), "PLAYER");
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
    levelClearBonus = 0;
    scoreSaved = false;
    hasKey = false;
}

bool GameStateData::isPlaying() const
{
    return state == STATE_PLAYING;
}
