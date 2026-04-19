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

namespace
{
    bool inRect(int px, int py, float rx, float ry, float rw, float rh)
    {
        return px >= static_cast<int>(rx) && px <= static_cast<int>(rx + rw)
            && py >= static_cast<int>(ry) && py <= static_cast<int>(ry + rh);
    }
}

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
    std::fprintf(f, "SOUND=%d\n", gameState.soundEnabled ? 1 : 0);
    std::fprintf(f, "NAME=%s\n", gameState.playerName);
    std::fclose(f);
}

void Game::loadSettings()
{
    FILE* f = std::fopen(Config::SETTINGS_FILE, "r");
    if (!f) return;
    char line[64];
    while (std::fgets(line, sizeof(line), f))
    {
        // strip trailing newline
        char* nl = std::strchr(line, '\n');
        if (nl) *nl = '\0';
        char* cr = std::strchr(line, '\r');
        if (cr) *cr = '\0';

        if (std::strncmp(line, "SOUND=", 6) == 0)
            gameState.soundEnabled = (line[6] == '1');
        else if (std::strncmp(line, "NAME=", 5) == 0 && line[5] != '\0')
            std::snprintf(gameState.playerName, sizeof(gameState.playerName), "%s", line + 5);
    }
    std::fclose(f);
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
        Menu::drawHowToPlay(windowWidth, windowHeight);
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
        Menu::drawPauseMenu(windowWidth, windowHeight);
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
    // Bounds check
    if (levelIndex < 0 || levelIndex >= Config::TOTAL_LEVELS)
    {
        levelIndex = 0;
    }
    
    gameState.currentLevelIndex = levelIndex;
    gameState.hasKey = false;
    
    // Use procedural generation for all levels
    level.loadProcedural(levelIndex);

    gameState.levelClearBonus = 0;
    timer.reset();
    timer.start();
    player.reset();
    gameState.state = STATE_PLAYING;
}

void Game::returnToMainMenu()
{
    player.resetMovementFlags();
    timer.reset();
    gameState.levelClearBonus = 0;
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
    int timeBonus = 1200 - timer.getElapsedMs() / 50;
    int lifeBonus = gameState.lives * 150;

    if (timeBonus < 0)
    {
        timeBonus = 0;
    }

    int baseLevelBonus = 1000 + gameState.currentLevelIndex * 500;
    gameState.levelClearBonus = baseLevelBonus + timeBonus + lifeBonus;
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

void Game::appendPlayerNameCharacter(unsigned char key)
{
    std::size_t length = std::strlen(gameState.playerName);

    if (key < 32 || key > 126 || key == '|' || length >= Config::MAX_PLAYER_NAME_LENGTH)
    {
        return;
    }

    if (key == ' ' && length == 0)
    {
        return;
    }

    gameState.playerName[length] = static_cast<char>(key);
    gameState.playerName[length + 1] = '\0';
}

void Game::removeLastPlayerNameCharacter()
{
    std::size_t length = std::strlen(gameState.playerName);

    if (length > 0)
    {
        gameState.playerName[length - 1] = '\0';
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
        else if (gameState.state == STATE_HOW_TO_PLAY || gameState.state == STATE_SETTINGS)
        {
            returnToMainMenu();
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
                gameState.state = STATE_SETTINGS;
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
        if (key == Config::KEY_ENTER)
        {
            saveSettings();
            returnToMainMenu();
        }
        else if (key == 's' || key == 'S')
        {
            gameState.soundEnabled = !gameState.soundEnabled;
        }
        else if (key == Config::KEY_BACKSPACE)
        {
            removeLastPlayerNameCharacter();
        }
        else
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
        float panelW = 600.0f, panelH = 500.0f;
        float panelX = (cachedWinW - panelW) / 2.0f;
        float panelY = (cachedWinH - panelH) / 2.0f;
        float optX = panelX + 40.0f;
        float optW = panelW - 80.0f;
        float optY = panelY + panelH - 160.0f;

        for (int i = 0; i < 4; i++)
        {
            float oy = optY - i * 50.0f;
            if (inRect(x, y, optX, oy, optW, 40.0f))
            {
                gameState.menuSelection = i;
                if (i == 0)      startNewRun(0);
                else if (i == 1) gameState.state = STATE_HOW_TO_PLAY;
                else if (i == 2) gameState.state = STATE_SETTINGS;
                else             glutLeaveMainLoop();
                return;
            }
        }
    }
    else if (gameState.state == STATE_SETTINGS)
    {
        float panelW = 560.0f, panelH = 360.0f;
        float panelX = (cachedWinW - panelW) / 2.0f;
        float panelY = (cachedWinH - panelH) / 2.0f;
        float optY = panelY + panelH - 120.0f;

        // Sound toggle row
        if (inRect(x, y, panelX + 40.0f, optY - 20.0f, panelW - 80.0f, 60.0f))
            gameState.soundEnabled = !gameState.soundEnabled;
    }
    else if (gameState.state == STATE_PAUSED)
    {
        float panelW = 400.0f, panelH = 320.0f;
        float panelX = (cachedWinW - panelW) / 2.0f;
        float panelY = (cachedWinH - panelH) / 2.0f;
        float btnX = panelX + 40.0f, btnW = panelW - 80.0f;
        float baseY = panelY + panelH - 120.0f;

        if      (inRect(x, y, btnX, baseY,        btnW, 34.0f)) resumeGame();
        else if (inRect(x, y, btnX, baseY - 44.0f, btnW, 34.0f)) startLevel(gameState.currentLevelIndex);
        else if (inRect(x, y, btnX, baseY - 88.0f, btnW, 34.0f)) returnToMainMenu();
        else if (inRect(x, y, btnX, baseY - 132.0f, btnW, 34.0f)) glutLeaveMainLoop();
    }
    else if (gameState.state == STATE_LEVEL_CLEARED)
    {
        float panelW = 420.0f, panelH = 160.0f;
        float panelX = (cachedWinW - panelW) / 2.0f;
        float panelY = (cachedWinH - panelH) / 2.0f;
        // Click anywhere on the overlay → next level
        if (inRect(x, y, panelX, panelY, panelW, panelH))
            startLevel(gameState.currentLevelIndex + 1);
    }
    else if (gameState.state == STATE_CAMPAIGN_WON || gameState.state == STATE_GAME_OVER)
    {
        float panelW = 460.0f, panelH = 340.0f;
        float panelX = (cachedWinW - panelW) / 2.0f;
        float panelY = (cachedWinH - panelH) / 2.0f;
        float btnY = panelY + 14.0f, btnH = 30.0f;

        // Replay button (left)
        if (inRect(x, y, panelX + 30.0f, btnY, 160.0f, btnH))
            startNewRun(gameState.selectedStartLevelIndex);
        // Menu button (right)
        else if (inRect(x, y, panelX + 210.0f, btnY, 160.0f, btnH))
            returnToMainMenu();
    }
}
