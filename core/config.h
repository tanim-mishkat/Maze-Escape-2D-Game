#ifndef CONFIG_H
#define CONFIG_H

// Shared layout metrics for the currently loaded board.
struct BoardMetrics
{
    int rows;
    int cols;
    float tileSize;
    float boardWidth;
    float boardHeight;
    float playerSize;
    float playerPadding;
    float boardPadding;
    float boardBorderWidth;
    float tileInnerPadding;
    float tileInnerSize;
    float sidePanelWidth;
    float canvasMargin;
    float sidebarMargin;

    BoardMetrics()
        : rows(0),
          cols(0),
          tileSize(0.0f),
          boardWidth(0.0f),
          boardHeight(0.0f),
          playerSize(0.0f),
          playerPadding(0.0f),
          boardPadding(0.0f),
          boardBorderWidth(0.0f),
          tileInnerPadding(0.0f),
          tileInnerSize(0.0f),
          sidePanelWidth(260.0f),
          canvasMargin(20.0f),
          sidebarMargin(24.0f)
    {
    }
};

// Maze and window configuration
namespace Config
{
    // Window dimensions
    constexpr int DEFAULT_WINDOW_WIDTH = 1180;
    constexpr int DEFAULT_WINDOW_HEIGHT = 720;
    constexpr float MAX_BOARD_PIXEL_WIDTH = 800.0f;
    constexpr float MAX_BOARD_PIXEL_HEIGHT = 600.0f;

    // HUD dimensions
    constexpr float SIDE_PANEL_WIDTH = 260.0f;

    // Game settings
    constexpr int TOTAL_LEVELS = 5;
    constexpr int MAX_LIVES = 3;
    constexpr int MAX_HIGH_SCORES = 5;
    constexpr int MAX_PLAYER_NAME_LENGTH = 18;
    constexpr char HIGH_SCORE_FILE[] = "maze_highscores.txt";
    constexpr int HOLD_MOVE_INITIAL_DELAY_MS = 170;
    constexpr int HOLD_MOVE_REPEAT_INTERVAL_MS = 95;
    constexpr int TIME_SCORE_WINDOW_MULTIPLIER = 2;
    constexpr int TIME_SCORE_DIVISOR_MS = 25;

    // Key codes
    constexpr unsigned char KEY_ESC = 27;
    constexpr unsigned char KEY_ENTER = 13;
    constexpr unsigned char KEY_BACKSPACE = 8;

    // Settings persistence
    constexpr char SETTINGS_FILE[] = "maze_settings.txt";

    // HUD card layout offsets (from panel top)
    constexpr float HUD_CARD_PLAYER_OFFSET = 140.0f;
    constexpr float HUD_CARD_STAGE_OFFSET = 224.0f;
    constexpr float HUD_CARD_SCORE_OFFSET = 302.0f;
    constexpr float HUD_CARD_LIVES_OFFSET = 302.0f;
    constexpr float HUD_CARD_PAR_OFFSET = 380.0f;
    constexpr float HUD_CARD_BEST_OFFSET = 380.0f;
    constexpr float HUD_CARD_HEIGHT_TALL = 72.0f;
    constexpr float HUD_CARD_HEIGHT_SHORT = 66.0f;

    inline float maxFloat(float a, float b)
    {
        return a > b ? a : b;
    }

    inline float minFloat(float a, float b)
    {
        return a < b ? a : b;
    }

    inline BoardMetrics makeBoardMetrics(int rows, int cols)
    {
        BoardMetrics metrics;
        metrics.rows = rows;
        metrics.cols = cols;
        metrics.sidePanelWidth = SIDE_PANEL_WIDTH;
        metrics.canvasMargin = 20.0f;
        metrics.sidebarMargin = 24.0f;

        float widthTile = MAX_BOARD_PIXEL_WIDTH / static_cast<float>(cols);
        float heightTile = MAX_BOARD_PIXEL_HEIGHT / static_cast<float>(rows);
        metrics.tileSize = minFloat(widthTile, heightTile);

        metrics.boardWidth = metrics.tileSize * static_cast<float>(cols);
        metrics.boardHeight = metrics.tileSize * static_cast<float>(rows);

        metrics.playerSize = maxFloat(10.0f, metrics.tileSize * 0.70f);
        if (metrics.playerSize > metrics.tileSize - 4.0f)
        {
            metrics.playerSize = metrics.tileSize - 4.0f;
        }
        metrics.playerPadding = (metrics.tileSize - metrics.playerSize) * 0.5f;

        metrics.boardPadding = maxFloat(2.5f, metrics.tileSize * 0.12f);
        metrics.boardBorderWidth = maxFloat(1.0f, metrics.tileSize * 0.035f);
        metrics.tileInnerPadding = maxFloat(1.0f, metrics.tileSize * 0.08f);
        metrics.tileInnerSize = metrics.tileSize - metrics.tileInnerPadding * 2.0f;

        return metrics;
    }
}

#endif // CONFIG_H
