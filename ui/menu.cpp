#include "menu.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include "../ui/hud.h"
#include <GL/freeglut.h>
#include <cstdio>

namespace Menu
{
    static bool inRect(int px, int py, float rx, float ry, float rw, float rh)
    {
        return px >= static_cast<int>(rx) && px <= static_cast<int>(rx + rw)
            && py >= static_cast<int>(ry) && py <= static_cast<int>(ry + rh);
    }

    static void drawMenuButton(float x, float y, float width, float height,
                               const char* text, bool selected, bool hovered)
    {
        float accentR, accentG, accentB, bgAlpha, outlineW, outlineA;
        if (selected)
        {
            accentR = 1.0f;  accentG = 0.88f; accentB = 0.42f;
            bgAlpha = 0.92f; outlineW = 2.5f;  outlineA = 0.90f;
        }
        else if (hovered)
        {
            accentR = 0.78f; accentG = 0.88f; accentB = 0.98f;
            bgAlpha = 0.82f; outlineW = 1.8f;  outlineA = 0.65f;
        }
        else
        {
            accentR = 0.5f;  accentG = 0.7f;  accentB = 0.9f;
            bgAlpha = 0.70f; outlineW = 1.0f;  outlineA = 0.30f;
        }

        Renderer::drawFilledRect(x, y, width, height, 0.04f, 0.06f, 0.09f, bgAlpha);
        Renderer::drawRectOutline(x, y, width, height, outlineW,
                                  accentR, accentG, accentB, outlineA);

        // Left accent bar on selected/hovered
        if (selected || hovered)
        {
            Renderer::drawFilledRect(x, y, 4.0f, height, accentR, accentG, accentB, 0.90f);
        }

        float textX = x + 20.0f;
        float textY = y + (height - 18.0f) / 2.0f + 2.0f;
        TextRenderer::drawTextWithShadow(textX, textY, text, accentR, accentG, accentB);
    }

    void drawMainMenu(int windowWidth, int windowHeight,
                      const GameStateData& gameState,
                      const HighScoreManager& highScores)
    {
        float panelW = 600.0f, panelH = 500.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float optX = panelX + 40.0f;
        float optW = panelW - 80.0f;
        float optY = panelY + panelH - 160.0f;
        float optH = 44.0f;
        float optGap = 54.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelW, panelH,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelW, panelH, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);
        // Top accent bar
        Renderer::drawFilledRect(panelX + 4.0f, panelY + panelH - 6.0f,
                                 panelW - 8.0f, 3.0f,
                                 Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                 Colors::ACCENT_BLUE_B, 0.80f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelH - 50.0f,
                                         "MAZE RUNNER",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelH - 78.0f,
                                         "Procedural Maze Adventure",
                                         0.86f, 0.98f, 0.84f);

        const char* labels[4] = {
            "  Start New Game",
            "  How to Play",
            "  Settings",
            "  Quit"
        };

        for (int i = 0; i < 4; i++)
        {
            float oy = optY - i * optGap;
            bool hov = inRect(gameState.mouseX, gameState.mouseY, optX, oy, optW, optH);
            drawMenuButton(optX, oy, optW, optH, labels[i],
                           gameState.menuSelection == i, hov);
        }

        // High score panel
        HUD::drawPanel(panelX + 40.0f, panelY + 30.0f, panelW - 80.0f, 90.0f,
                       0.45f, 0.68f, 0.96f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, panelY + 94.0f,
                                         "Best Score:", 0.98f, 0.92f, 0.58f);
        char scoreText[32];
        std::snprintf(scoreText, sizeof(scoreText), "%d", highScores.getBestScore());
        TextRenderer::drawTextWithShadow(panelX + 180.0f, panelY + 94.0f,
                                         scoreText, 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, panelY + 60.0f,
                                         "Arrow keys or 1-4 to select,  Enter / Click to confirm",
                                         0.74f, 0.84f, 1.0f);
    }

    void drawPauseMenu(int windowWidth, int windowHeight)
    {
        float panelW = 400.0f, panelH = 320.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float btnX = panelX + 40.0f, btnW = panelW - 80.0f, btnH = 34.0f;
        float baseY = panelY + panelH - 120.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 0.0f, 0.0f, 0.0f, 0.60f);

        Renderer::drawFilledRect(panelX, panelY, panelW, panelH,
                                 0.03f, 0.05f, 0.08f, 0.96f);
        Renderer::drawRectOutline(panelX, panelY, panelW, panelH, 2.5f,
                                  0.96f, 0.84f, 0.30f, 0.80f);

        float titleW = static_cast<float>(TextRenderer::getTextWidth("PAUSED"));
        TextRenderer::drawTextWithShadow(panelX + (panelW - titleW) / 2.0f,
                                         panelY + panelH - 48.0f,
                                         "PAUSED",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        struct { const char* label; float dy; } btns[4] = {
            { "  Resume  (P / ESC)", 0.0f },
            { "  Restart Level  (R)", -44.0f },
            { "  Main Menu  (M)", -88.0f },
            { "  Quit  (Q)", -132.0f }
        };

        for (int i = 0; i < 4; i++)
        {
            float by = baseY + btns[i].dy;
            Renderer::drawFilledRect(btnX, by, btnW, btnH, 0.04f, 0.06f, 0.10f, 0.80f);
            Renderer::drawRectOutline(btnX, by, btnW, btnH, 1.2f,
                                      0.60f, 0.80f, 1.0f, 0.40f);
            TextRenderer::drawTextWithShadow(btnX + 8.0f, by + 9.0f,
                                             btns[i].label, 0.90f, 0.94f, 1.0f);
        }

        TextRenderer::drawTextWithShadow(panelX + 20.0f, panelY + 14.0f,
                                         "Click a button or use keyboard",
                                         0.60f, 0.70f, 0.90f);
    }

    void drawHowToPlay(int windowWidth, int windowHeight)
    {
        float panelW = 700.0f, panelH = 560.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelW, panelH,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelW, panelH, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);

        // Title — positioned WELL clear of the first section panel below it
        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelH - 44.0f,
                                         "HOW TO PLAY",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        // Sections — start 30px lower than before to avoid overlap with title
        float textY = panelY + panelH - 112.0f;

        HUD::drawPanel(panelX + 40.0f, textY - 20.0f, panelW - 80.0f, 78.0f,
                       0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY,
                                         "OBJECTIVE", 0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 30.0f,
                                         "Find the golden key, then reach the exit flag!",
                                         0.92f, 0.95f, 1.0f);

        textY -= 118.0f;

        HUD::drawPanel(panelX + 40.0f, textY - 20.0f, panelW - 80.0f, 140.0f,
                       0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY,
                                         "CONTROLS", 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 30.0f,
                                         "WASD or Arrow Keys — Move one tile",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 58.0f,
                                         "P — Pause game",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 86.0f,
                                         "R — Restart current level",
                                         0.92f, 0.95f, 1.0f);

        textY -= 176.0f;

        HUD::drawPanel(panelX + 40.0f, textY - 20.0f, panelW - 80.0f, 110.0f,
                       0.98f, 0.82f, 0.44f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY,
                                         "HAZARDS", 0.98f, 0.82f, 0.44f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 30.0f,
                                         "Red Traps — Lose 1 life and 125 points",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 58.0f,
                                         "Brown Obstacles — Block your path",
                                         0.92f, 0.95f, 1.0f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 22.0f,
                                         "Press ESC or Enter to return",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
    }

    void drawSettings(int windowWidth, int windowHeight,
                      const GameStateData& gameState)
    {
        float panelW = 560.0f, panelH = 360.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float optY = panelY + panelH - 120.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelW, panelH,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelW, panelH, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelH - 44.0f,
                                         "SETTINGS",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        // --- Sound Effects row ---
        float rowW = panelW - 80.0f;
        bool soundHov = inRect(gameState.mouseX, gameState.mouseY,
                               panelX + 40.0f, optY - 20.0f, rowW, 60.0f);
        float soundBorderA = soundHov ? 0.70f : 0.42f;
        HUD::drawPanel(panelX + 40.0f, optY - 20.0f, rowW, 60.0f,
                       0.58f, 0.90f, 0.98f, soundBorderA);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, optY,
                                         "Sound Effects", 0.58f, 0.90f, 0.98f);
        const char* soundStatus = gameState.soundEnabled ? "ON" : "OFF";
        float sndR = gameState.soundEnabled ? 0.40f : 0.95f;
        float sndG = gameState.soundEnabled ? 0.95f : 0.50f;
        float sndB = gameState.soundEnabled ? 0.50f : 0.50f;
        TextRenderer::drawTextWithShadow(panelX + rowW - 50.0f, optY,
                                         soundStatus, sndR, sndG, sndB);
        if (soundHov)
        {
            TextRenderer::drawTextWithShadow(panelX + 60.0f, optY - 26.0f,
                                             "Click or press S to toggle",
                                             0.72f, 0.80f, 0.90f);
        }

        optY -= 100.0f;

        // --- Player Name row (active input field) ---
        HUD::drawPanel(panelX + 40.0f, optY - 20.0f, rowW, 80.0f,
                       0.70f, 0.95f, 0.74f, 0.55f);
        // Highlighted active-field border
        Renderer::drawRectOutline(panelX + 40.0f, optY - 20.0f, rowW, 80.0f, 2.0f,
                                  0.40f, 0.95f, 0.60f, 0.90f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, optY + 24.0f,
                                         "Player Name", 0.70f, 0.95f, 0.74f);

        // Name text with blinking cursor
        int elapsed = glutGet(GLUT_ELAPSED_TIME);
        bool showCursor = (elapsed / 500) % 2 == 0;
        char nameDisplay[Config::MAX_PLAYER_NAME_LENGTH + 3];
        std::snprintf(nameDisplay, sizeof(nameDisplay), "%s%s",
                      gameState.playerName, showCursor ? "|" : " ");
        TextRenderer::drawTextWithShadow(panelX + 60.0f, optY - 8.0f,
                                         nameDisplay, 0.95f, 0.98f, 1.0f);

        // Hint row
        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 56.0f,
                                         "S — toggle sound     Type to edit name     Backspace to delete",
                                         0.72f, 0.80f, 0.92f);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 28.0f,
                                         "Press Enter to save and return",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
    }
}
