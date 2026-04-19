#ifndef CONFIG_H
#define CONFIG_H

// Maze and window configuration
namespace Config
{
    // Grid dimensions
    constexpr int MAZE_ROWS = 12;
    constexpr int MAZE_COLS = 16;
    constexpr int TILE_SIZE = 50;
    constexpr int BOARD_WIDTH = MAZE_COLS * TILE_SIZE;
    constexpr int BOARD_HEIGHT = MAZE_ROWS * TILE_SIZE;

    // Window dimensions
    constexpr int SIDE_PANEL_WIDTH = 260;
    constexpr int DEFAULT_WINDOW_WIDTH = BOARD_WIDTH + SIDE_PANEL_WIDTH + 120;
    constexpr int DEFAULT_WINDOW_HEIGHT = BOARD_HEIGHT + 120;

    // Player settings
    constexpr float PLAYER_SIZE = 30.0f;
    constexpr float PLAYER_PADDING = 10.0f;

    // Game settings
    constexpr int TOTAL_LEVELS = 5;
    constexpr int MAX_LIVES = 3;
    constexpr int MAX_HIGH_SCORES = 5;
    constexpr int MAX_PLAYER_NAME_LENGTH = 18;
    constexpr int TRAP_PENALTY = 125;
    constexpr int KEY_BONUS = 200;
    constexpr char HIGH_SCORE_FILE[] = "maze_highscores.txt";

    // Start position (consistent across all levels)
    constexpr int START_ROW = 9;
    constexpr int START_COL = 1;

    // Procedural generation settings
    constexpr float WALL_DENSITY_BASE = 0.35f;
    constexpr float WALL_DENSITY_INCREMENT = 0.05f;
    constexpr int MIN_PATH_LENGTH = 15;
    
    // Key codes
    constexpr unsigned char KEY_ESC   = 27;
    constexpr unsigned char KEY_ENTER = 13;
    constexpr unsigned char KEY_BACKSPACE = 8;

    // Settings persistence
    constexpr char SETTINGS_FILE[] = "maze_settings.txt";

    // Rendering constants
    constexpr float BOARD_PADDING = 6.0f;
    constexpr float BOARD_BORDER_WIDTH = 1.5f;
    constexpr float CANVAS_MARGIN = 20.0f;
    constexpr float SIDEBAR_MARGIN = 24.0f;
    constexpr float TILE_INNER_PADDING = 4.0f;
    constexpr float TILE_INNER_SIZE = TILE_SIZE - 8.0f;

    // HUD card layout offsets (from panel top)
    constexpr float HUD_CARD_PLAYER_OFFSET  = 140.0f;
    constexpr float HUD_CARD_STAGE_OFFSET   = 224.0f;
    constexpr float HUD_CARD_SCORE_OFFSET   = 302.0f;
    constexpr float HUD_CARD_LIVES_OFFSET   = 302.0f;
    constexpr float HUD_CARD_KEY_OFFSET     = 380.0f;
    constexpr float HUD_CARD_BEST_OFFSET    = 380.0f;
    constexpr float HUD_CARD_HEIGHT_TALL    = 72.0f;
    constexpr float HUD_CARD_HEIGHT_SHORT   = 66.0f;
}

#endif // CONFIG_H
