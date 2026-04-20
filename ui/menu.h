#ifndef MENU_H
#define MENU_H

#include "../core/gamestate.h"
#include "../data/highscore.h"

namespace Menu
{
    enum MainMenuAction
    {
        MAIN_MENU_ACTION_NONE = -1,
        MAIN_MENU_ACTION_START = 0,
        MAIN_MENU_ACTION_HOW_TO_PLAY = 1,
        MAIN_MENU_ACTION_SETTINGS = 2,
        MAIN_MENU_ACTION_QUIT = 3
    };

    enum PauseMenuAction
    {
        PAUSE_MENU_ACTION_NONE = -1,
        PAUSE_MENU_ACTION_RESUME = 0,
        PAUSE_MENU_ACTION_RESTART = 1,
        PAUSE_MENU_ACTION_MENU = 2,
        PAUSE_MENU_ACTION_QUIT = 3
    };

    enum SettingsAction
    {
        SETTINGS_ACTION_NONE = -1,
        SETTINGS_ACTION_EDIT = 0,
        SETTINGS_ACTION_SAVE = 1,
        SETTINGS_ACTION_CANCEL = 2,
        SETTINGS_ACTION_BACK = 3
    };

    RectF getMainMenuButtonRect(int windowWidth, int windowHeight, int actionIndex);
    MainMenuAction hitTestMainMenu(int windowWidth, int windowHeight, int mouseX, int mouseY);

    RectF getPauseMenuButtonRect(int windowWidth, int windowHeight, int actionIndex);
    PauseMenuAction hitTestPauseMenu(int windowWidth, int windowHeight, int mouseX, int mouseY);

    RectF getHowToPlayBackButtonRect(int windowWidth, int windowHeight);
    bool isHowToPlayBackHit(int windowWidth, int windowHeight, int mouseX, int mouseY);

    RectF getSettingsNameFieldRect(int windowWidth, int windowHeight);
    RectF getSettingsButtonRect(int windowWidth, int windowHeight, int buttonIndex);
    SettingsAction hitTestSettings(int windowWidth, int windowHeight,
                                   const GameStateData& gameState,
                                   int mouseX, int mouseY);

    void drawMainMenu(int windowWidth, int windowHeight,
                      const GameStateData& gameState,
                      const HighScoreManager& highScores);
    void drawPauseMenu(int windowWidth, int windowHeight,
                       const GameStateData& gameState);
    void drawHowToPlay(int windowWidth, int windowHeight,
                       const GameStateData& gameState);
    void drawSettings(int windowWidth, int windowHeight,
                      const GameStateData& gameState);
}

#endif // MENU_H
