#include "game.h"
#include "../gameplay/collision.h"
#include "../render/renderer.h"
#include "../render/colors.h"
#include "../ui/hud.h"
#include "../ui/menu.h"
#include "../ui/overlay.h"
#include <GL/freeglut.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>

Game::Game() : cachedWinW(1180), cachedWinH(720)
{
}

void Game::init()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    loadSettings();
    highScores.load();
    returnToMainMenu();
}

void Game::saveSettings()
{
    FILE* f = std::fopen(Config::SETTINGS_FILE, "w");
    if (!f) return;
    std::fprintf(f, "NAME=%s\n", gameState.playerName);
    std::fclose(f);
}

void Game::loadSettings()
{
    FILE* f = std::fopen(Config::SETTINGS_FILE, "r");
    if (!f)
    {
        ensurePlayerName();
        syncPlayerNameToSettingsDraft();
        return;
    }
    char line[64];
    while (std::fgets(line, sizeof(line), f))
    {
        // strip trailing newline
        char* nl = std::strchr(line, '\n');
        if (nl) *nl = '\0';
        char* cr = std::strchr(line, '\r');
        if (cr) *cr = '\0';

        if (std::strncmp(line, "NAME=", 5) == 0 && line[5] != '\0')
            std::snprintf(gameState.playerName, sizeof(gameState.playerName), "%s", line + 5);
    }
    std::fclose(f);
    ensurePlayerName();
    syncPlayerNameToSettingsDraft();
}

void Game::update()
{
    timer.updateAnimationTime();
}

void Game::render(int windowWidth, int windowHeight)
{
    cachedWinW = windowWidth;
    cachedWinH = windowHeight;
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gameState.state == STATE_MAIN_MENU)
    {
        Menu::drawMainMenu(windowWidth, windowHeight, gameState, highScores);
        return;
    }

    if (gameState.state == STATE_HOW_TO_PLAY)
    {
        Menu::drawHowToPlay(windowWidth, windowHeight, gameState);
        return;
    }

    if (gameState.state == STATE_SETTINGS)
    {
        Menu::drawSettings(windowWidth, windowHeight, gameState);
        return;
    }

    // Draw game background
    Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                            static_cast<float>(windowHeight),
                            Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

    // Draw main game panel
    HUD::drawPanel(Renderer::getGameCanvasX(windowWidth),
                  Renderer::getGameCanvasY(windowHeight),
                  Renderer::getGameCanvasWidth(),
                  Renderer::getGameCanvasHeight(),
                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G, Colors::ACCENT_BLUE_B, 0.78f);

    // Draw board frame
    float boardX = Renderer::getBoardOriginX(windowWidth) - Config::BOARD_PADDING;
    float boardY = Renderer::getBoardOriginY(windowHeight) - Config::BOARD_PADDING;
    float boardFrameWidth = Config::BOARD_WIDTH + Config::BOARD_PADDING * 2.0f;
    float boardFrameHeight = Config::BOARD_HEIGHT + Config::BOARD_PADDING * 2.0f;
    
    Renderer::drawFilledRect(boardX, boardY, boardFrameWidth, boardFrameHeight,
                            0.01f, 0.02f, 0.04f, 0.86f);
    Renderer::drawRectOutline(boardX, boardY, boardFrameWidth, boardFrameHeight,
                             Config::BOARD_BORDER_WIDTH,
                             Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                             Colors::ACCENT_BLUE_B, 0.26f);

    // Draw game elements
    glPushMatrix();
    glTranslatef(Renderer::getBoardOriginX(windowWidth),
                Renderer::getBoardOriginY(windowHeight), 0.0f);
    Renderer::drawFloor();
    Renderer::drawMaze(level.getMaze());
    Renderer::drawExit(level.getMaze(), timer.getAnimationTime(), gameState.hasKey);
    Renderer::drawTraps(level.getMaze(), timer.getAnimationTime());
    Renderer::drawObstacles(level.getMaze());
    Renderer::drawKeys(level.getMaze(), timer.getAnimationTime());
    Renderer::drawPlayer(player);
    glPopMatrix();

    // Draw HUD
    HUD::drawHud(windowWidth, windowHeight, gameState, level, timer, highScores);

    // Draw overlays
    if (gameState.state == STATE_PAUSED)
    {
        Menu::drawPauseMenu(windowWidth, windowHeight, gameState);
    }
    else if (gameState.state == STATE_LEVEL_CLEARED)
    {
        Overlay::drawLevelClearOverlay(windowWidth, windowHeight, gameState, level);
    }
    else if (gameState.state == STATE_CAMPAIGN_WON)
    {
        Overlay::drawEndOverlay(windowWidth, windowHeight,
                               "Victory!",
                               "You completed all levels!",
                               gameState, highScores);
    }
    else if (gameState.state == STATE_GAME_OVER)
    {
        Overlay::drawEndOverlay(windowWidth, windowHeight,
                               "Game Over",
                               "You ran out of lives",
                               gameState, highScores);
    }
}

void Game::startNewRun(int levelIndex)
{
    ensurePlayerName();
    gameState.selectedStartLevelIndex = levelIndex;
    gameState.menuSelection = levelIndex;
    gameState.score = 0;
    gameState.lives = Config::MAX_LIVES;
    gameState.scoreSaved = false;
    gameState.hasKey = false;
    startLevel(levelIndex);
}

void Game::startLevel(int levelIndex)
{
    if (levelIndex < 0 || levelIndex >= Config::TOTAL_LEVELS)
    {
        levelIndex = 0;
    }

    gameState.currentLevelIndex = levelIndex;
    gameState.hasKey = false;

    level.load(levelIndex);
    gameState.levelClearBonus = 0;
    timer.reset();
    timer.start();
    player.reset();
    gameState.state = STATE_PLAYING;
}

void Game::openSettings()
{
    syncPlayerNameToSettingsDraft();
    gameState.settingsEditingName = false;
    gameState.state = STATE_SETTINGS;
}

void Game::returnToMainMenu()
{
    player.resetMovementFlags();
    timer.reset();
    gameState.levelClearBonus = 0;
    gameState.settingsEditingName = false;
    syncPlayerNameToSettingsDraft();
    gameState.titleStep = TITLE_STEP_WELCOME;
    gameState.menuSelection = 0;
    gameState.state = STATE_MAIN_MENU;
}

void Game::pauseGame()
{
    if (gameState.state == STATE_PLAYING)
    {
        gameState.state = STATE_PAUSED;
        timer.stop();
    }
}

void Game::resumeGame()
{
    if (gameState.state == STATE_PAUSED)
    {
        gameState.state = STATE_PLAYING;
        timer.resume();
    }
}

void Game::tryMoveByTile(int colStep, int rowStep)
{
    if (!gameState.isPlaying())
    {
        return;
    }

    GridPos currentPos = player.getGridPos();
    GridPos targetPos(currentPos.row + rowStep, currentPos.col + colStep);

    if (!Collision::canMoveTo(level.getMaze(), targetPos))
    {
        return;
    }

    player.setPosition(targetPos);

    // Check for key pickup
    if (Collision::isAtKey(level.getMaze(), player))
    {
        handleKeyPickup();
    }
    // Check for trap
    else if (Collision::isAtTrap(level.getMaze(), player))
    {
        handleTrapHit();
    }
    // Check for exit
    else if (Collision::isAtExit(level.getMaze(), player, gameState.hasKey))
    {
        handleExitReached();
    }
}

void Game::handleKeyPickup()
{
    gameState.hasKey = true;
    gameState.score += Config::KEY_BONUS;
    
    // Remove key from maze
    GridPos playerPos = player.getGridPos();
    level.getMaze().removeTile(playerPos.row, playerPos.col);
}

void Game::handleTrapHit()
{
    if (gameState.score >= Config::TRAP_PENALTY)
    {
        gameState.score -= Config::TRAP_PENALTY;
    }
    else
    {
        gameState.score = 0;
    }

    gameState.lives--;

    if (gameState.lives <= 0)
    {
        gameState.lives = 0;
        gameState.state = STATE_GAME_OVER;
        player.resetMovementFlags();
        finalizeSessionIfNeeded();
        return;
    }

    player.reset();
}

void Game::handleExitReached()
{
    const LevelDefinition& definition = level.getDefinition();
    int timeBonus = definition.parTimeMs - timer.getElapsedMs();
    int lifeBonus = gameState.lives * 175;

    if (timeBonus < 0)
    {
        timeBonus = 0;
    }

    gameState.levelClearBonus = definition.clearBonus + (timeBonus / 25) + lifeBonus;
    gameState.score += gameState.levelClearBonus;
    player.resetMovementFlags();

    if (gameState.currentLevelIndex == Config::TOTAL_LEVELS - 1)
    {
        gameState.state = STATE_CAMPAIGN_WON;
        finalizeSessionIfNeeded();
    }
    else
    {
        gameState.state = STATE_LEVEL_CLEARED;
    }
}

void Game::finalizeSessionIfNeeded()
{
    if (gameState.scoreSaved)
    {
        return;
    }

    ensurePlayerName();
    highScores.add(gameState.playerName, gameState.score);
    gameState.scoreSaved = true;
}

void Game::syncSettingsDraftToPlayerName()
{
    std::snprintf(gameState.playerName, sizeof(gameState.playerName), "%s", gameState.settingsDraftName);
}

void Game::syncPlayerNameToSettingsDraft()
{
    std::snprintf(gameState.settingsDraftName, sizeof(gameState.settingsDraftName), "%s", gameState.playerName);
}

void Game::beginNameEdit()
{
    syncPlayerNameToSettingsDraft();
    gameState.settingsEditingName = true;
}

void Game::saveNameEdit()
{
    syncSettingsDraftToPlayerName();
    ensurePlayerName();
    syncPlayerNameToSettingsDraft();
    saveSettings();
    gameState.settingsEditingName = false;
}

void Game::cancelNameEdit()
{
    syncPlayerNameToSettingsDraft();
    gameState.settingsEditingName = false;
}

void Game::appendPlayerNameCharacter(unsigned char key)
{
    std::size_t length = std::strlen(gameState.settingsDraftName);

    if (key < 32 || key > 126 || key == '|' || length >= Config::MAX_PLAYER_NAME_LENGTH)
    {
        return;
    }

    if (key == ' ' && length == 0)
    {
        return;
    }

    gameState.settingsDraftName[length] = static_cast<char>(key);
    gameState.settingsDraftName[length + 1] = '\0';
}

void Game::removeLastPlayerNameCharacter()
{
    std::size_t length = std::strlen(gameState.settingsDraftName);

    if (length > 0)
    {
        gameState.settingsDraftName[length - 1] = '\0';
    }
}

void Game::ensurePlayerName()
{
    if (gameState.playerName[0] == '\0')
    {
        std::snprintf(gameState.playerName, sizeof(gameState.playerName), "PLAYER");
    }
}

void Game::handleKeyDown(unsigned char key)
{
    if (key == Config::KEY_ESC)
    {
        if (gameState.state == STATE_PLAYING)
        {
            pauseGame();
            return;
        }
        else if (gameState.state == STATE_PAUSED)
        {
            resumeGame();
            return;
        }
        else if (gameState.state == STATE_HOW_TO_PLAY)
        {
            returnToMainMenu();
            return;
        }
        else if (gameState.state == STATE_SETTINGS)
        {
            if (gameState.settingsEditingName)
            {
                cancelNameEdit();
            }
            else
            {
                returnToMainMenu();
            }
            return;
        }
        else if (gameState.state == STATE_MAIN_MENU)
        {
            glutLeaveMainLoop();
            return;
        }
    }

    // Main menu handling
    if (gameState.state == STATE_MAIN_MENU)
    {
        if (key == '1')
        {
            gameState.menuSelection = 0;
        }
        else if (key == '2')
        {
            gameState.menuSelection = 1;
        }
        else if (key == '3')
        {
            gameState.menuSelection = 2;
        }
        else if (key == '4')
        {
            gameState.menuSelection = 3;
        }
        else if (key == Config::KEY_ENTER)
        {
            if (gameState.menuSelection == 0)
            {
                startNewRun(0);
            }
            else if (gameState.menuSelection == 1)
            {
                gameState.state = STATE_HOW_TO_PLAY;
            }
            else if (gameState.menuSelection == 2)
            {
                openSettings();
            }
            else if (gameState.menuSelection == 3)
            {
                glutLeaveMainLoop();
            }
        }
        return;
    }

    // How to play / settings
    if (gameState.state == STATE_HOW_TO_PLAY)
    {
        if (key == Config::KEY_ENTER)
        {
            returnToMainMenu();
        }
        return;
    }

    if (gameState.state == STATE_SETTINGS)
    {
        if (!gameState.settingsEditingName && (key == 'e' || key == 'E'))
        {
            beginNameEdit();
        }
        else if (!gameState.settingsEditingName && key == Config::KEY_ENTER)
        {
            beginNameEdit();
        }
        else if (gameState.settingsEditingName && key == Config::KEY_ENTER)
        {
            saveNameEdit();
        }
        else if (gameState.settingsEditingName && key == Config::KEY_BACKSPACE)
        {
            removeLastPlayerNameCharacter();
        }
        else if (gameState.settingsEditingName)
        {
            appendPlayerNameCharacter(key);
        }
        return;
    }

    // Pause menu
    if (gameState.state == STATE_PAUSED)
    {
        if (key == 'p' || key == 'P')
        {
            resumeGame();
        }
        else if (key == 'r' || key == 'R')
        {
            startLevel(gameState.currentLevelIndex);
        }
        else if (key == 'm' || key == 'M')
        {
            returnToMainMenu();
        }
        else if (key == 'q' || key == 'Q')
        {
            glutLeaveMainLoop();
        }
        return;
    }

    // In-game keys
    if (key == 'p' || key == 'P')
    {
        if (gameState.state == STATE_PLAYING)
        {
            pauseGame();
        }
        return;
    }
    
    if (key == 'r' || key == 'R')
    {
        if (gameState.state == STATE_PLAYING || gameState.state == STATE_GAME_OVER
            || gameState.state == STATE_CAMPAIGN_WON)
        {
            startNewRun(gameState.selectedStartLevelIndex);
        }
        return;
    }

    if (key == 'q' || key == 'Q')
    {
        if (gameState.state == STATE_GAME_OVER || gameState.state == STATE_CAMPAIGN_WON)
        {
            glutLeaveMainLoop();
        }
        return;
    }

    if (key == 'm' || key == 'M')
    {
        returnToMainMenu();
        return;
    }
    
    if ((key == 'n' || key == 'N' || key == 13) && gameState.state == STATE_LEVEL_CLEARED)
    {
        startLevel(gameState.currentLevelIndex + 1);
        return;
    }

    if (!gameState.isPlaying())
    {
        return;
    }

    // Movement keys
    if ((key == 'w' || key == 'W') && !player.isMoveUp())
    {
        player.setMoveUp(true);
        tryMoveByTile(0, -1);
    }
    else if ((key == 's' || key == 'S') && !player.isMoveDown())
    {
        player.setMoveDown(true);
        tryMoveByTile(0, 1);
    }
    else if ((key == 'a' || key == 'A') && !player.isMoveLeft())
    {
        player.setMoveLeft(true);
        tryMoveByTile(-1, 0);
    }
    else if ((key == 'd' || key == 'D') && !player.isMoveRight())
    {
        player.setMoveRight(true);
        tryMoveByTile(1, 0);
    }
}

void Game::handleKeyUp(unsigned char key)
{
    if (key == 'w' || key == 'W')
    {
        player.setMoveUp(false);
    }
    else if (key == 's' || key == 'S')
    {
        player.setMoveDown(false);
    }
    else if (key == 'a' || key == 'A')
    {
        player.setMoveLeft(false);
    }
    else if (key == 'd' || key == 'D')
    {
        player.setMoveRight(false);
    }
}

void Game::handleSpecialDown(int key)
{
    // Main menu navigation
    if (gameState.state == STATE_MAIN_MENU)
    {
        if (key == GLUT_KEY_UP)
        {
            gameState.menuSelection = (gameState.menuSelection + 3) % 4;
        }
        else if (key == GLUT_KEY_DOWN)
        {
            gameState.menuSelection = (gameState.menuSelection + 1) % 4;
        }
        return;
    }

    if (!gameState.isPlaying())
    {
        return;
    }

    if (key == GLUT_KEY_UP && !player.isMoveUp())
    {
        player.setMoveUp(true);
        tryMoveByTile(0, -1);
    }
    else if (key == GLUT_KEY_DOWN && !player.isMoveDown())
    {
        player.setMoveDown(true);
        tryMoveByTile(0, 1);
    }
    else if (key == GLUT_KEY_LEFT && !player.isMoveLeft())
    {
        player.setMoveLeft(true);
        tryMoveByTile(-1, 0);
    }
    else if (key == GLUT_KEY_RIGHT && !player.isMoveRight())
    {
        player.setMoveRight(true);
        tryMoveByTile(1, 0);
    }
}

void Game::handleSpecialUp(int key)
{
    if (key == GLUT_KEY_UP)
    {
        player.setMoveUp(false);
    }
    else if (key == GLUT_KEY_DOWN)
    {
        player.setMoveDown(false);
    }
    else if (key == GLUT_KEY_LEFT)
    {
        player.setMoveLeft(false);
    }
    else if (key == GLUT_KEY_RIGHT)
    {
        player.setMoveRight(false);
    }
}

void Game::handleMouseMove(int x, int y)
{
    gameState.mouseX = x;
    gameState.mouseY = y;
}

void Game::handleMouseClick(int x, int y)
{
    gameState.mouseX = x;
    gameState.mouseY = y;

    if (gameState.state == STATE_MAIN_MENU)
    {
        Menu::MainMenuAction action = Menu::hitTestMainMenu(cachedWinW, cachedWinH, x, y);

        if (action != Menu::MAIN_MENU_ACTION_NONE)
        {
            gameState.menuSelection = static_cast<int>(action);

            if (action == Menu::MAIN_MENU_ACTION_START)
            {
                startNewRun(0);
            }
            else if (action == Menu::MAIN_MENU_ACTION_HOW_TO_PLAY)
            {
                gameState.state = STATE_HOW_TO_PLAY;
            }
            else if (action == Menu::MAIN_MENU_ACTION_SETTINGS)
            {
                openSettings();
            }
            else if (action == Menu::MAIN_MENU_ACTION_QUIT)
            {
                glutLeaveMainLoop();
            }

            return;
        }
    }
    else if (gameState.state == STATE_HOW_TO_PLAY)
    {
        if (Menu::isHowToPlayBackHit(cachedWinW, cachedWinH, x, y))
        {
            returnToMainMenu();
        }
    }
    else if (gameState.state == STATE_SETTINGS)
    {
        Menu::SettingsAction action = Menu::hitTestSettings(cachedWinW, cachedWinH, gameState, x, y);

        if (action == Menu::SETTINGS_ACTION_EDIT)
        {
            beginNameEdit();
        }
        else if (action == Menu::SETTINGS_ACTION_SAVE)
        {
            saveNameEdit();
        }
        else if (action == Menu::SETTINGS_ACTION_CANCEL)
        {
            cancelNameEdit();
        }
        else if (action == Menu::SETTINGS_ACTION_BACK)
        {
            returnToMainMenu();
        }
    }
    else if (gameState.state == STATE_PAUSED)
    {
        Menu::PauseMenuAction action = Menu::hitTestPauseMenu(cachedWinW, cachedWinH, x, y);

        if (action == Menu::PAUSE_MENU_ACTION_RESUME)
        {
            resumeGame();
        }
        else if (action == Menu::PAUSE_MENU_ACTION_RESTART)
        {
            startLevel(gameState.currentLevelIndex);
        }
        else if (action == Menu::PAUSE_MENU_ACTION_MENU)
        {
            returnToMainMenu();
        }
        else if (action == Menu::PAUSE_MENU_ACTION_QUIT)
        {
            glutLeaveMainLoop();
        }
    }
    else if (gameState.state == STATE_LEVEL_CLEARED)
    {
        if (Overlay::isLevelClearContinueHit(cachedWinW, cachedWinH, x, y))
            startLevel(gameState.currentLevelIndex + 1);
    }
    else if (gameState.state == STATE_CAMPAIGN_WON || gameState.state == STATE_GAME_OVER)
    {
        Overlay::EndOverlayAction action = Overlay::hitTestEndOverlay(cachedWinW, cachedWinH, x, y);

        if (action == Overlay::END_OVERLAY_ACTION_REPLAY)
            startNewRun(gameState.selectedStartLevelIndex);
        else if (action == Overlay::END_OVERLAY_ACTION_MENU)
            returnToMainMenu();
        else if (action == Overlay::END_OVERLAY_ACTION_QUIT)
            glutLeaveMainLoop();
    }
}
