#include "game.h"
#include "input.h"
#include "../gameplay/collision.h"
#include "../render/renderer.h"
#include "../render/colors.h"
#include "../ui/hud.h"
#include "../ui/menu.h"
#include "../ui/overlay.h"
#include <GL/freeglut.h>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <random>
#include <ctime>

Game::Game()
    : cachedWinW(1180),
      cachedWinH(720),
      inputState() // InputManager::State is initialized with default constructor
{
}

Game::~Game()
{
    // Destructor will be called when game goes out of scope
    // GLUT-owned resources (bitmap fonts) are cleaned up by GLUT
}

void Game::shutdown()
{
    // Explicit cleanup - called before program exit
    // Save any state that needs persistence
    settings.save();
    highScores.save();

    // Note: OpenGL resources (fonts, etc.) are owned by GLUT/system
    // and will be cleaned up automatically on program termination
}

void Game::init()
{
    // Initialize random number generator using Mersenne Twister
    // (Used by maze generation)
    std::random_device rd;
    srand(rd());

    // Load settings from persistent storage
    settings.load();
    std::strncpy(gameState.playerName, settings.getPlayerName(),
                 Config::MAX_PLAYER_NAME_LENGTH);
    gameState.playerName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';

    highScores.load();
    returnToMainMenu();
}

void Game::beginNameEdit()
{
    // Initialize draft name from current settings
    settings.resetDraftName();
    std::strncpy(gameState.settingsDraftName, settings.getDraftName(),
                 Config::MAX_PLAYER_NAME_LENGTH);
    gameState.settingsDraftName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
    gameState.settingsEditingName = true;
}
{
    timer.updateAnimationTime();
    updateHeldMovement();
}

void Game::render(int windowWidth, int windowHeight)
{
    cachedWinW = windowWidth;
    cachedWinH = windowHeight;
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const BoardMetrics &metrics = level.getMetrics();

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
    HUD::drawPanel(Renderer::getGameCanvasX(windowWidth, metrics),
                   Renderer::getGameCanvasY(windowHeight, metrics),
                   Renderer::getGameCanvasWidth(metrics),
                   Renderer::getGameCanvasHeight(metrics),
                   Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G, Colors::ACCENT_BLUE_B, 0.78f);

    // Draw board frame
    float boardX = Renderer::getBoardOriginX(windowWidth, metrics) - metrics.boardPadding;
    float boardY = Renderer::getBoardOriginY(windowHeight, metrics) - metrics.boardPadding;
    float boardFrameWidth = metrics.boardWidth + metrics.boardPadding * 2.0f;
    float boardFrameHeight = metrics.boardHeight + metrics.boardPadding * 2.0f;

    Renderer::drawFilledRect(boardX, boardY, boardFrameWidth, boardFrameHeight,
                             0.01f, 0.02f, 0.04f, 0.86f);
    Renderer::drawRectOutline(boardX, boardY, boardFrameWidth, boardFrameHeight,
                              metrics.boardBorderWidth,
                              Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                              Colors::ACCENT_BLUE_B, 0.26f);

    // Draw game elements
    glPushMatrix();
    glTranslatef(Renderer::getBoardOriginX(windowWidth, metrics),
                 Renderer::getBoardOriginY(windowHeight, metrics), 0.0f);
    Renderer::drawFloor(metrics);
    Renderer::drawMaze(level.getMaze());
    Renderer::drawExit(level.getMaze(), timer.getAnimationTime());
    Renderer::drawPlayer(player, metrics);
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
}

void Game::startNewRun(int levelIndex)
{
    ensurePlayerName();
    gameState.selectedStartLevelIndex = levelIndex;
    gameState.menuSelection = levelIndex;
    gameState.score = 0;
    gameState.lastLevelScore = 0;
    gameState.lastLevelTimeMs = 0;
    gameState.scoreSaved = false;
    startLevel(levelIndex);
}

void Game::startLevel(int levelIndex)
{
    // Validate level index - clamp to valid range
    if (levelIndex < 0 || levelIndex >= Config::TOTAL_LEVELS)
    {
        levelIndex = 0;
    }

    gameState.currentLevelIndex = levelIndex;

    // Load level (which internally clamps index again, but we've already validated)
    level.load(levelIndex);

    // Reset level state
    gameState.lastLevelScore = 0;
    gameState.lastLevelTimeMs = 0;
    timer.reset();
    timer.start();

    // Reset player at maze start position
    GridPos startPos = level.getMaze().getStartPos();
    player.reset(startPos);

    clearMoveRepeatState();
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
    clearMoveRepeatState();
    timer.reset();
    gameState.lastLevelScore = 0;
    gameState.lastLevelTimeMs = 0;
    gameState.settingsEditingName = false;
    syncPlayerNameToSettingsDraft();
    gameState.menuSelection = 0;
    gameState.state = STATE_MAIN_MENU;
}

void Game::pauseGame()
{
    if (gameState.state == STATE_PLAYING)
    {
        clearMoveRepeatState();
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

    if (Collision::isAtExit(level.getMaze(), player))
    {
        handleExitReached();
    }
}

int Game::computeStageScore(int elapsedMs, int parTimeMs) const
{
    int timeWindowMs = parTimeMs * Config::TIME_SCORE_WINDOW_MULTIPLIER;
    int scoreValue = timeWindowMs - elapsedMs;

    if (scoreValue < 0)
    {
        scoreValue = 0;
    }

    return scoreValue / Config::TIME_SCORE_DIVISOR_MS;
}

void Game::clearMoveRepeatState()
{
    inputState.reset();

    player.resetMovementFlags();
}

// ============================================================================
// INPUT STATE MACHINE: "Most Recently Pressed Key Wins"
// ============================================================================
//
// Movement input is handled using a priority system that gives preference
// to the most recently pressed direction key. This provides intuitive
// keyboard handling where overlapping key presses don't cause ambiguity.
//
// ALGORITHM:
//   1. Track which keys are currently held: movePressOrder[4]
//      - movePressOrder[UP/DOWN/LEFT/RIGHT] = press sequence counter
//      - Value 0 means not held; higher values = more recent press
//   2. When a key is pressed:
//      - Increment global movePressSequence counter
//      - Store current sequence number in movePressOrder[direction]
//   3. When a key is released:
//      - Set movePressOrder[direction] = 0
//   4. To find active direction:
//      - Among all held directions, pick the one with highest sequence number
//      - This is the most recently pressed key
//   5. Apply movement with repeat:
//      - Initial press: move immediately, set repeat timer (Config::HOLD_MOVE_INITIAL_DELAY_MS)
//      - Held key: repeat movement at interval (Config::HOLD_MOVE_REPEAT_INTERVAL_MS)
//      - Change direction: reset timer to initial delay
//
// EXAMPLE:
//   User presses: UP → RIGHT (while UP held) → move UP initially
//   → system detects RIGHT more recent, switches to move RIGHT
//   → user releases UP → continue moving RIGHT
//   → user presses UP (while RIGHT held) → switch to move UP
//
// ============================================================================

void Game::markMoveDirectionHeld(MoveDirection direction, bool held)
{
    if (direction == InputManager::MOVE_DIRECTION_NONE)
    {
        return;
    }

    bool wasHeld = false;
    switch (direction)
    {
    case InputManager::MOVE_DIRECTION_UP:
        wasHeld = player.isMoveUp();
        player.setMoveUp(held);
        break;
    case InputManager::MOVE_DIRECTION_DOWN:
        wasHeld = player.isMoveDown();
        player.setMoveDown(held);
        break;
    case InputManager::MOVE_DIRECTION_LEFT:
        wasHeld = player.isMoveLeft();
        player.setMoveLeft(held);
        break;
    case InputManager::MOVE_DIRECTION_RIGHT:
        wasHeld = player.isMoveRight();
        player.setMoveRight(held);
        break;
    default:
        return;
    }

    if (held)
    {
        if (!wasHeld)
        {
            inputState.movePressOrder[direction] = ++inputState.movePressSequence;
        }
    }
    else
    {
        inputState.movePressOrder[direction] = 0;
    }
}

MoveDirection Game::getActiveHeldDirection() const
{
    MoveDirection bestDirection = InputManager::MOVE_DIRECTION_NONE;
    int bestOrder = -1;

    if (player.isMoveUp() && inputState.movePressOrder[InputManager::MOVE_DIRECTION_UP] > bestOrder)
    {
        bestDirection = InputManager::MOVE_DIRECTION_UP;
        bestOrder = inputState.movePressOrder[InputManager::MOVE_DIRECTION_UP];
    }
    if (player.isMoveDown() && inputState.movePressOrder[InputManager::MOVE_DIRECTION_DOWN] > bestOrder)
    {
        bestDirection = InputManager::MOVE_DIRECTION_DOWN;
        bestOrder = inputState.movePressOrder[InputManager::MOVE_DIRECTION_DOWN];
    }
    if (player.isMoveLeft() && inputState.movePressOrder[InputManager::MOVE_DIRECTION_LEFT] > bestOrder)
    {
        bestDirection = InputManager::MOVE_DIRECTION_LEFT;
        bestOrder = inputState.movePressOrder[InputManager::MOVE_DIRECTION_LEFT];
    }
    if (player.isMoveRight() && inputState.movePressOrder[InputManager::MOVE_DIRECTION_RIGHT] > bestOrder)
    {
        bestDirection = InputManager::MOVE_DIRECTION_RIGHT;
    }

    return bestDirection;
}

void Game::applyMoveDirection(MoveDirection direction, bool isInitialMove)
{
    if (!gameState.isPlaying())
    {
        clearMoveRepeatState();
        return;
    }

    if (direction == InputManager::MOVE_DIRECTION_NONE)
    {
        inputState.repeatedMoveDirection = InputManager::MOVE_DIRECTION_NONE;
        inputState.nextRepeatedMoveMs = 0;
        return;
    }

    if (direction == InputManager::MOVE_DIRECTION_UP)
    {
        tryMoveByTile(0, -1);
    }
    else if (direction == InputManager::MOVE_DIRECTION_DOWN)
    {
        tryMoveByTile(0, 1);
    }
    else if (direction == InputManager::MOVE_DIRECTION_LEFT)
    {
        tryMoveByTile(-1, 0);
    }
    else if (direction == InputManager::MOVE_DIRECTION_RIGHT)
    {
        tryMoveByTile(1, 0);
    }

    if (!gameState.isPlaying())
    {
        clearMoveRepeatState();
        return;
    }

    repeatedMoveDirection = getActiveHeldDirection();
    if (repeatedMoveDirection == InputManager::MOVE_DIRECTION_NONE)
    {
        nextRepeatedMoveMs = 0;
        return;
    }

    nextRepeatedMoveMs = glutGet(GLUT_ELAPSED_TIME) + InputManager::getRepeatDelayMs(isInitialMove);
}

void Game::updateHeldMovement()
{
    if (!gameState.isPlaying())
    {
        return;
    }

    MoveDirection activeDirection = getActiveHeldDirection();
    if (activeDirection == InputManager::MOVE_DIRECTION_NONE)
    {
        inputState.repeatedMoveDirection = InputManager::MOVE_DIRECTION_NONE;
        inputState.nextRepeatedMoveMs = 0;
        return;
    }

    int now = glutGet(GLUT_ELAPSED_TIME);
    if (inputState.repeatedMoveDirection != activeDirection)
    {
        inputState.repeatedMoveDirection = activeDirection;
        inputState.nextRepeatedMoveMs = now + Config::HOLD_MOVE_INITIAL_DELAY_MS;
        return;
    }

    if (inputState.nextRepeatedMoveMs == 0)
    {
        inputState.nextRepeatedMoveMs = now + Config::HOLD_MOVE_INITIAL_DELAY_MS;
        return;
    }

    if (now >= inputState.nextRepeatedMoveMs)
    {
        applyMoveDirection(activeDirection, false);
    }
}

void Game::handleExitReached()
{
    const LevelSpec &definition = level.getDefinition();
    timer.stop();
    gameState.lastLevelTimeMs = timer.getElapsedMs();
    gameState.lastLevelScore = computeStageScore(gameState.lastLevelTimeMs, definition.parTimeMs);
    gameState.score += gameState.lastLevelScore;
    clearMoveRepeatState();

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

void Game::saveNameEdit()
{
    // Update settings with edited name
    settings.setDraftName(gameState.settingsDraftName);
    settings.confirmDraftName(); // This copies draft to playerName and saves

    // Sync back to gameState
    std::strncpy(gameState.playerName, settings.getPlayerName(),
                 Config::MAX_PLAYER_NAME_LENGTH);
    gameState.playerName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
    gameState.settingsEditingName = false;
}

void Game::cancelNameEdit()
{
    // Reload draft from current player name
    settings.resetDraftName();
    std::strncpy(gameState.settingsDraftName, settings.getDraftName(),
                 Config::MAX_PLAYER_NAME_LENGTH);
    gameState.settingsDraftName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
    gameState.settingsEditingName = false;
}

void Game::appendPlayerNameCharacter(unsigned char key)
{
    // Get current length of draft name in gameState
    std::size_t length = std::strlen(gameState.settingsDraftName);

    // Validate: printable char, not pipe (used as separator), not space at start, and room in buffer
    // Ensure length + 1 (new char) + 1 (null terminator) fits in buffer
    if (key < 32 || key > 126 || key == '|' || length >= Config::MAX_PLAYER_NAME_LENGTH)
    {
        return;
    }

    if (key == ' ' && length == 0)
    {
        return; // Don't start name with space
    }

    // Safe to append: we know length < MAX_PLAYER_NAME_LENGTH
    gameState.settingsDraftName[length] = static_cast<char>(key);
    gameState.settingsDraftName[length + 1] = '\0'; // Always null-terminate

    // Also update settings draft name to keep them in sync
    settings.setDraftName(gameState.settingsDraftName);
}

void Game::removeLastPlayerNameCharacter()
{
    std::size_t length = std::strlen(gameState.settingsDraftName);

    if (length > 0)
    {
        gameState.settingsDraftName[length - 1] = '\0';
        // Also update settings draft name to keep them in sync
        settings.setDraftName(gameState.settingsDraftName);
    }
}

void Game::syncPlayerNameToSettingsDraft()
{
    std::snprintf(gameState.settingsDraftName, sizeof(gameState.settingsDraftName), "%s", gameState.playerName);
    settings.setDraftName(gameState.settingsDraftName);
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
        if (gameState.state == STATE_PLAYING || gameState.state == STATE_CAMPAIGN_WON)
        {
            startNewRun(gameState.selectedStartLevelIndex);
        }
        return;
    }

    if (key == 'q' || key == 'Q')
    {
        if (gameState.state == STATE_CAMPAIGN_WON)
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

    if (key == 'w' || key == 'W')
    {
        if (!player.isMoveUp())
        {
            markMoveDirectionHeld(MOVE_DIRECTION_UP, true);
            applyMoveDirection(MOVE_DIRECTION_UP, true);
        }
    }
    else if (key == 's' || key == 'S')
    {
        if (!player.isMoveDown())
        {
            markMoveDirectionHeld(MOVE_DIRECTION_DOWN, true);
            applyMoveDirection(MOVE_DIRECTION_DOWN, true);
        }
    }
    else if (key == 'a' || key == 'A')
    {
        if (!player.isMoveLeft())
        {
            markMoveDirectionHeld(MOVE_DIRECTION_LEFT, true);
            applyMoveDirection(MOVE_DIRECTION_LEFT, true);
        }
    }
    else if (key == 'd' || key == 'D')
    {
        if (!player.isMoveRight())
        {
            markMoveDirectionHeld(MOVE_DIRECTION_RIGHT, true);
            applyMoveDirection(MOVE_DIRECTION_RIGHT, true);
        }
    }
}

void Game::handleKeyUp(unsigned char key)
{
    if (key == 'w' || key == 'W')
    {
        markMoveDirectionHeld(MOVE_DIRECTION_UP, false);
    }
    else if (key == 's' || key == 'S')
    {
        markMoveDirectionHeld(MOVE_DIRECTION_DOWN, false);
    }
    else if (key == 'a' || key == 'A')
    {
        markMoveDirectionHeld(MOVE_DIRECTION_LEFT, false);
    }
    else if (key == 'd' || key == 'D')
    {
        markMoveDirectionHeld(MOVE_DIRECTION_RIGHT, false);
    }

    MoveDirection activeDirection = getActiveHeldDirection();
    repeatedMoveDirection = activeDirection;
    if (activeDirection == MOVE_DIRECTION_NONE)
    {
        nextRepeatedMoveMs = 0;
    }
    else
    {
        nextRepeatedMoveMs = glutGet(GLUT_ELAPSED_TIME) + Config::HOLD_MOVE_INITIAL_DELAY_MS;
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
        markMoveDirectionHeld(MOVE_DIRECTION_UP, true);
        applyMoveDirection(MOVE_DIRECTION_UP, true);
    }
    else if (key == GLUT_KEY_DOWN && !player.isMoveDown())
    {
        markMoveDirectionHeld(MOVE_DIRECTION_DOWN, true);
        applyMoveDirection(MOVE_DIRECTION_DOWN, true);
    }
    else if (key == GLUT_KEY_LEFT && !player.isMoveLeft())
    {
        markMoveDirectionHeld(MOVE_DIRECTION_LEFT, true);
        applyMoveDirection(MOVE_DIRECTION_LEFT, true);
    }
    else if (key == GLUT_KEY_RIGHT && !player.isMoveRight())
    {
        markMoveDirectionHeld(MOVE_DIRECTION_RIGHT, true);
        applyMoveDirection(MOVE_DIRECTION_RIGHT, true);
    }
}

void Game::handleSpecialUp(int key)
{
    if (key == GLUT_KEY_UP)
    {
        markMoveDirectionHeld(MOVE_DIRECTION_UP, false);
    }
    else if (key == GLUT_KEY_DOWN)
    {
        markMoveDirectionHeld(MOVE_DIRECTION_DOWN, false);
    }
    else if (key == GLUT_KEY_LEFT)
    {
        markMoveDirectionHeld(MOVE_DIRECTION_LEFT, false);
    }
    else if (key == GLUT_KEY_RIGHT)
    {
        markMoveDirectionHeld(MOVE_DIRECTION_RIGHT, false);
    }

    MoveDirection activeDirection = getActiveHeldDirection();
    repeatedMoveDirection = activeDirection;
    if (activeDirection == MOVE_DIRECTION_NONE)
    {
        nextRepeatedMoveMs = 0;
    }
    else
    {
        nextRepeatedMoveMs = glutGet(GLUT_ELAPSED_TIME) + Config::HOLD_MOVE_INITIAL_DELAY_MS;
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
    else if (gameState.state == STATE_CAMPAIGN_WON)
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
