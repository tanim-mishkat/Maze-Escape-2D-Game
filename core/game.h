#ifndef GAME_H
#define GAME_H

#include "gamestate.h"
#include "../gameplay/player.h"
#include "../gameplay/level.h"
#include "../data/highscore.h"
#include "../utils/timer.h"

class Game
{
public:
    Game();

    void init();
    void setWindowSize(int w, int h) { cachedWinW = w; cachedWinH = h; }
    void update();
    void render(int windowWidth, int windowHeight);

    // Input handling
    void handleKeyDown(unsigned char key);
    void handleKeyUp(unsigned char key);
    void handleSpecialDown(int key);
    void handleSpecialUp(int key);
    void handleMouseClick(int x, int y);  // x,y in OpenGL coords (y=0 at bottom)
    void handleMouseMove(int x, int y);

    // Game flow
    void startNewRun(int levelIndex);
    void startLevel(int levelIndex);
    void returnToMainMenu();
    void pauseGame();
    void resumeGame();

    // Player name management
    void beginNameEdit();
    void saveNameEdit();
    void cancelNameEdit();
    void appendPlayerNameCharacter(unsigned char key);
    void removeLastPlayerNameCharacter();
    void ensurePlayerName();

    // Settings persistence
    void saveSettings();
    void loadSettings();

    // Window size (cached from last render call, used by mouse handlers)
    int cachedWinW, cachedWinH;

    // Getters
    const GameStateData& getState() const { return gameState; }
    const Player& getPlayer() const { return player; }
    const Level& getLevel() const { return level; }
    const Timer& getTimer() const { return timer; }
    const HighScoreManager& getHighScores() const { return highScores; }

private:
    enum MoveDirection
    {
        MOVE_DIRECTION_NONE = -1,
        MOVE_DIRECTION_UP = 0,
        MOVE_DIRECTION_DOWN = 1,
        MOVE_DIRECTION_LEFT = 2,
        MOVE_DIRECTION_RIGHT = 3
    };

    GameStateData gameState;
    Player player;
    Level level;
    Timer timer;
    HighScoreManager highScores;
    int movePressSequence;
    int movePressOrder[4];
    MoveDirection repeatedMoveDirection;
    int nextRepeatedMoveMs;

    // Game logic
    void tryMoveByTile(int colStep, int rowStep);
    void handleExitReached();
    void finalizeSessionIfNeeded();
    void openSettings();
    void syncSettingsDraftToPlayerName();
    void syncPlayerNameToSettingsDraft();
    int computeStageScore(int elapsedMs, int parTimeMs) const;
    void clearMoveRepeatState();
    void markMoveDirectionHeld(MoveDirection direction, bool held);
    MoveDirection getActiveHeldDirection() const;
    void applyMoveDirection(MoveDirection direction, bool allowImmediateMove);
    void updateHeldMovement();
};

#endif // GAME_H
