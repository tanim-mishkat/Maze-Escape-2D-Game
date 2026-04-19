#include "menu.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include "../ui/hud.h"
#include <cstdio>

namespace Menu
{
    static void drawMenuOption(float x, float y, float width, const char* text,
                               bool selected)
    {
        float accentR = selected ? 1.0f : 0.5f;
        float accentG = selected ? 0.88f : 0.7f;
        float accentB = selected ? 0.42f : 0.9f;

        Renderer::drawFilledRect(x, y, width, 40.0f, 0.04f, 0.06f, 0.09f,
                                 selected ? 0.92f : 0.70f);
        Renderer::drawRectOutline(x, y, width, 40.0f, selected ? 2.5f : 1.0f,
                                  accentR, accentG, accentB, selected ? 0.90f : 0.30f);
        TextRenderer::drawTextWithShadow(x + 20.0f, y + 12.0f, text,
                                         accentR, accentG, accentB);
    }

    void drawMainMenu(int windowWidth, int windowHeight,
                      const GameStateData& gameState,
                      const HighScoreManager& highScores)
    {
        float panelWidth = 600.0f;
        float panelHeight = 500.0f;
        float panelX = (windowWidth - panelWidth) / 2.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelWidth, panelHeight,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelWidth, panelHeight, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelHeight - 50.0f,
                                         "MAZE RUNNER",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelHeight - 80.0f,
                                         "Procedural Maze Adventure",
                                         0.86f, 0.98f, 0.84f);

        float optionY = panelY + panelHeight - 160.0f;
        float optionWidth = panelWidth - 80.0f;

        drawMenuOption(panelX + 40.0f, optionY, optionWidth,
                       "1. Start New Game", gameState.menuSelection == 0);
        drawMenuOption(panelX + 40.0f, optionY - 50.0f, optionWidth,
                       "2. How to Play", gameState.menuSelection == 1);
        drawMenuOption(panelX + 40.0f, optionY - 100.0f, optionWidth,
                       "3. Settings", gameState.menuSelection == 2);
        drawMenuOption(panelX + 40.0f, optionY - 150.0f, optionWidth,
                       "4. Quit", gameState.menuSelection == 3);

        HUD::drawPanel(panelX + 40.0f, panelY + 30.0f, panelWidth - 80.0f, 100.0f,
                       0.45f, 0.68f, 0.96f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, panelY + 100.0f,
                                         "High Score:", 0.98f, 0.92f, 0.58f);

        char scoreText[32];
        std::snprintf(scoreText, sizeof(scoreText), "%d", highScores.getBestScore());
        TextRenderer::drawTextWithShadow(panelX + 60.0f, panelY + 70.0f,
                                         scoreText, 0.70f, 0.95f, 0.74f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelHeight - 460.0f,
                                         "Use arrow keys or 1-4 to select, Enter to confirm",
                                         0.82f, 0.90f, 1.0f);
    }

    void drawPauseMenu(int windowWidth, int windowHeight)
    {
        float panelWidth = 400.0f;
        float panelHeight = 300.0f;
        float panelX = (windowWidth - panelWidth) / 2.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 0.0f, 0.0f, 0.0f, 0.60f);

        Renderer::drawFilledRect(panelX, panelY, panelWidth, panelHeight,
                                 0.03f, 0.05f, 0.08f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelWidth, panelHeight, 2.5f,
                                  0.96f, 0.84f, 0.30f, 0.80f);

        TextRenderer::drawTextWithShadow(panelX + (panelWidth - TextRenderer::getTextWidth("PAUSED")) / 2.0f,
                                         panelY + panelHeight - 50.0f,
                                         "PAUSED",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        float optionY = panelY + panelHeight - 120.0f;
        TextRenderer::drawTextWithShadow(panelX + 40.0f, optionY,
                                         "P or ESC - Resume", 0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, optionY - 30.0f,
                                         "R - Restart Level", 0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, optionY - 60.0f,
                                         "M - Main Menu", 0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, optionY - 90.0f,
                                         "Q - Quit Game", 0.92f, 0.95f, 1.0f);
    }

    void drawHowToPlay(int windowWidth, int windowHeight)
    {
        float panelWidth = 700.0f;
        float panelHeight = 550.0f;
        float panelX = (windowWidth - panelWidth) / 2.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelWidth, panelHeight,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelWidth, panelHeight, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelHeight - 50.0f,
                                         "HOW TO PLAY",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        float textY = panelY + panelHeight - 100.0f;

        HUD::drawPanel(panelX + 40.0f, textY - 20.0f, panelWidth - 80.0f, 80.0f,
                       0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY,
                                         "OBJECTIVE", 0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 30.0f,
                                         "Find the golden key, then reach the exit flag!",
                                         0.92f, 0.95f, 1.0f);

        textY -= 120.0f;

        HUD::drawPanel(panelX + 40.0f, textY - 20.0f, panelWidth - 80.0f, 140.0f,
                       0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY,
                                         "CONTROLS", 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 30.0f,
                                         "WASD or Arrow Keys - Move one tile",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 60.0f,
                                         "P - Pause game",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 90.0f,
                                         "R - Restart current level",
                                         0.92f, 0.95f, 1.0f);

        textY -= 180.0f;

        HUD::drawPanel(panelX + 40.0f, textY - 20.0f, panelWidth - 80.0f, 110.0f,
                       0.98f, 0.82f, 0.44f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY,
                                         "HAZARDS", 0.98f, 0.82f, 0.44f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 30.0f,
                                         "Red Traps - Lose 1 life and 125 points",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, textY - 60.0f,
                                         "Brown Obstacles - Block your path",
                                         0.92f, 0.95f, 1.0f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 40.0f,
                                         "Press ESC or Enter to return to main menu",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
    }

    void drawSettings(int windowWidth, int windowHeight,
                      const GameStateData& gameState)
    {
        float panelWidth = 500.0f;
        float panelHeight = 350.0f;
        float panelX = (windowWidth - panelWidth) / 2.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelWidth, panelHeight,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelWidth, panelHeight, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelHeight - 50.0f,
                                         "SETTINGS",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        float optionY = panelY + panelHeight - 120.0f;

        HUD::drawPanel(panelX + 40.0f, optionY - 20.0f, panelWidth - 80.0f, 60.0f,
                       0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, optionY,
                                         "Sound Effects", 0.58f, 0.90f, 0.98f);
        const char* soundStatus = gameState.soundEnabled ? "ON" : "OFF";
        TextRenderer::drawTextWithShadow(panelX + 300.0f, optionY,
                                         soundStatus, 0.92f, 0.95f, 1.0f);

        optionY -= 100.0f;

        HUD::drawPanel(panelX + 40.0f, optionY - 20.0f, panelWidth - 80.0f, 60.0f,
                       0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, optionY,
                                         "Player Name", 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, optionY - 30.0f,
                                         gameState.playerName, 0.92f, 0.95f, 1.0f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 60.0f,
                                         "Press ESC or Enter to return",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 30.0f,
                                         "(Settings are for display only)",
                                         0.82f, 0.90f, 1.0f);
    }
}
