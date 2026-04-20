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
    constexpr float HUD_CARD_PAR_OFFSET = 380.0f;
    constexpr float HUD_CARD_BEST_OFFSET = 380.0f;
    constexpr float HUD_CARD_HEIGHT_TALL = 72.0f;
    constexpr float HUD_CARD_HEIGHT_SHORT = 66.0f;

    // ============= VISUAL RENDERING CONSTANTS =============

    // Floor tile shading
    constexpr float FLOOR_SHADE_DARK = 0.10f;
    constexpr float FLOOR_SHADE_LIGHT = 0.12f;
    constexpr float FLOOR_SHADE_GRADIENT = 0.02f;

    // Exit tile animation and appearance
    constexpr float EXIT_PULSE_AMPLITUDE = 0.5f;
    constexpr float EXIT_PULSE_FREQUENCY = 4.0f;   // cycles per second (approximately)
    constexpr float EXIT_GLOW_INSET_RATIO = 0.16f; // ratio of tile size for glow inset
    constexpr float EXIT_OUTER_RING_RATIO = 0.30f; // outer ring size ratio
    constexpr float EXIT_INNER_RING_RATIO = 0.12f; // inner ring size ratio

    // Maze wall rendering
    constexpr float WALL_COLOR_R = 0.20f;
    constexpr float WALL_COLOR_G = 0.22f;
    constexpr float WALL_COLOR_B = 0.28f;
    constexpr float WALL_ALPHA = 0.95f;

    // HUD panel dimensions
    constexpr float HUD_PANEL_WIDTH = 4.0f;
    constexpr float HUD_PANEL_HEIGHT = 8.0f;
    constexpr float HUD_PANEL_CORNER_RADIUS = 1.5f;

    // ============= MAZE GENERATION CONSTANTS =============

    // Scoring weights for maze candidate evaluation
    // These weights determine how much each factor influences difficulty
    constexpr int SCORE_CRITICAL_PATH_WEIGHT = 12;        // Direct path length importance
    constexpr int SCORE_TURN_COUNT_WEIGHT = 15;           // Path complexity from turns
    constexpr int SCORE_DEAD_END_WEIGHT = 8;              // Dead end count impact
    constexpr int SCORE_MAX_BRANCH_DEPTH_WEIGHT = 16;     // Maximum branch depth impact
    constexpr int SCORE_SECOND_ORDER_BRANCH_WEIGHT = 5;   // Secondary branching impact
    constexpr int SCORE_NEAR_CRITICAL_BRANCH_WEIGHT = 4;  // Branches near critical path
    constexpr int SCORE_JUNCTION_COUNT_WEIGHT = 14;       // Number of decision points
    constexpr int SCORE_ROUTE_DETOUR_WEIGHT = 10;         // Route detour factor
    constexpr int SCORE_LONGEST_STRAIGHT_RUN_WEIGHT = 18; // Straight corridors (longer = simpler)
    constexpr int SCORE_EXIT_BIAS_WEIGHT = 2;             // Exit placement bias

    // Maze generation parameters
    constexpr int MIN_CANDIDATE_COUNT = 28;
    constexpr int MAX_CANDIDATE_COUNT = 64;
    constexpr int ENHANCEMENT_ITERATIONS = 5;
    constexpr int MIN_BRANCH_COUNT = 2;
    constexpr int MAX_BRANCH_DEPTH = 4;

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
