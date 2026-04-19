#ifndef TYPES_H
#define TYPES_H

// Tile types in the maze grid
enum TileType
{
    TILE_PATH = 0,
    TILE_WALL = 1,
    TILE_TRAP = 2,
    TILE_OBSTACLE = 3,
    TILE_KEY = 4,
    TILE_LOCKED_EXIT = 5
};

// Game state machine
enum GameState
{
    STATE_MAIN_MENU,
    STATE_HOW_TO_PLAY,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_LEVEL_CLEARED,
    STATE_CAMPAIGN_WON,
    STATE_GAME_OVER,
    STATE_SETTINGS
};

// Title screen flow steps
enum TitleStep
{
    TITLE_STEP_WELCOME,
    TITLE_STEP_NAME,
    TITLE_STEP_DIFFICULTY
};

// Grid position
struct GridPos
{
    int row;
    int col;

    GridPos() : row(0), col(0) {}
    GridPos(int r, int c) : row(r), col(c) {}
};

// Screen position
struct ScreenPos
{
    float x;
    float y;

    ScreenPos() : x(0.0f), y(0.0f) {}
    ScreenPos(float px, float py) : x(px), y(py) {}
};

#endif // TYPES_H
