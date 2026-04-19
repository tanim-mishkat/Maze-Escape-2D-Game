#include "gamestate.h"
#include <cstring>

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
      soundEnabled(true)
{
    std::strcpy(playerName, "PLAYER");
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
