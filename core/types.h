#ifndef TYPES_H
#define TYPES_H

// Tile types in the maze grid
enum TileType
{
    TILE_PATH = 0,
    TILE_WALL = 1
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
    STATE_SETTINGS
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

struct RectF
{
    float x;
    float y;
    float width;
    float height;

    RectF() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    RectF(float px, float py, float w, float h)
        : x(px), y(py), width(w), height(h) {}

    bool contains(int px, int py) const
    {
        return px >= static_cast<int>(x) && px <= static_cast<int>(x + width)
            && py >= static_cast<int>(y) && py <= static_cast<int>(y + height);
    }
};

#endif // TYPES_H
