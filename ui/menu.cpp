#include "menu.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include "../ui/hud.h"
#include <GL/freeglut.h>
#include <cstdio>

namespace Menu
{
    namespace
    {
        void drawMenuButton(float x, float y, float width, float height,
                            const char* text, bool selected, bool hovered)
        {
            float accentR, accentG, accentB, bgAlpha, outlineW, outlineA;

            if (selected)
            {
                accentR = 1.0f;
                accentG = 0.88f;
                accentB = 0.42f;
                bgAlpha = 0.92f;
                outlineW = 2.5f;
                outlineA = 0.90f;
            }
            else if (hovered)
            {
                accentR = 0.78f;
                accentG = 0.88f;
                accentB = 0.98f;
                bgAlpha = 0.82f;
                outlineW = 1.8f;
                outlineA = 0.65f;
            }
            else
            {
                accentR = 0.50f;
                accentG = 0.70f;
                accentB = 0.90f;
                bgAlpha = 0.70f;
                outlineW = 1.0f;
                outlineA = 0.30f;
            }

            Renderer::drawFilledRect(x, y, width, height, 0.04f, 0.06f, 0.09f, bgAlpha);
            Renderer::drawRectOutline(x, y, width, height, outlineW,
                                      accentR, accentG, accentB, outlineA);

            if (selected || hovered)
            {
                Renderer::drawFilledRect(x, y, 4.0f, height, accentR, accentG, accentB, 0.90f);
            }

            TextRenderer::drawTextWithShadow(x + 20.0f,
                                             y + (height - 18.0f) / 2.0f + 2.0f,
                                             text, accentR, accentG, accentB);
        }
    }

    RectF getMainMenuButtonRect(int windowWidth, int windowHeight, int actionIndex)
    {
        float panelW = 600.0f;
        float panelH = 500.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float optX = panelX + 40.0f;
        float optW = panelW - 80.0f;
        float optY = panelY + panelH - 160.0f;
        float optH = 44.0f;
        float optGap = 54.0f;

        return RectF(optX, optY - actionIndex * optGap, optW, optH);
    }

    MainMenuAction hitTestMainMenu(int windowWidth, int windowHeight, int mouseX, int mouseY)
    {
        for (int i = 0; i < 4; i++)
        {
            if (getMainMenuButtonRect(windowWidth, windowHeight, i).contains(mouseX, mouseY))
            {
                return static_cast<MainMenuAction>(i);
            }
        }

        return MAIN_MENU_ACTION_NONE;
    }

    RectF getPauseMenuButtonRect(int windowWidth, int windowHeight, int actionIndex)
    {
        float panelW = 400.0f;
        float panelH = 320.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float btnX = panelX + 40.0f;
        float btnW = panelW - 80.0f;
        float btnH = 34.0f;
        float baseY = panelY + panelH - 120.0f;

        return RectF(btnX, baseY - actionIndex * 44.0f, btnW, btnH);
    }

    PauseMenuAction hitTestPauseMenu(int windowWidth, int windowHeight, int mouseX, int mouseY)
    {
        for (int i = 0; i < 4; i++)
        {
            if (getPauseMenuButtonRect(windowWidth, windowHeight, i).contains(mouseX, mouseY))
            {
                return static_cast<PauseMenuAction>(i);
            }
        }

        return PAUSE_MENU_ACTION_NONE;
    }

    RectF getHowToPlayBackButtonRect(int windowWidth, int windowHeight)
    {
        float panelW = 760.0f;
        float panelH = 620.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;

        return RectF(panelX + panelW - 240.0f, panelY + 18.0f, 200.0f, 34.0f);
    }

    bool isHowToPlayBackHit(int windowWidth, int windowHeight, int mouseX, int mouseY)
    {
        return getHowToPlayBackButtonRect(windowWidth, windowHeight).contains(mouseX, mouseY);
    }

    RectF getSettingsNameFieldRect(int windowWidth, int windowHeight)
    {
        float panelW = 580.0f;
        float panelH = 430.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;

        return RectF(panelX + 40.0f, panelY + 180.0f, panelW - 80.0f, 120.0f);
    }

    RectF getSettingsButtonRect(int windowWidth, int windowHeight, int buttonIndex)
    {
        float panelW = 580.0f;
        float panelH = 430.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float btnY = panelY + 32.0f;
        float btnW = 220.0f;
        float btnH = 36.0f;
        float btnGap = 20.0f;

        return RectF(panelX + 40.0f + buttonIndex * (btnW + btnGap), btnY, btnW, btnH);
    }

    SettingsAction hitTestSettings(int windowWidth, int windowHeight,
                                   const GameStateData& gameState,
                                   int mouseX, int mouseY)
    {
        if (!gameState.settingsEditingName
            && getSettingsNameFieldRect(windowWidth, windowHeight).contains(mouseX, mouseY))
        {
            return SETTINGS_ACTION_EDIT;
        }

        if (getSettingsButtonRect(windowWidth, windowHeight, 0).contains(mouseX, mouseY))
        {
            return gameState.settingsEditingName ? SETTINGS_ACTION_SAVE : SETTINGS_ACTION_EDIT;
        }

        if (getSettingsButtonRect(windowWidth, windowHeight, 1).contains(mouseX, mouseY))
        {
            return gameState.settingsEditingName ? SETTINGS_ACTION_CANCEL : SETTINGS_ACTION_BACK;
        }

        return SETTINGS_ACTION_NONE;
    }

    void drawMainMenu(int windowWidth, int windowHeight,
                      const GameStateData& gameState,
                      const HighScoreManager& highScores)
    {
        float panelW = 600.0f;
        float panelH = 500.0f;
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
            "  Start New Game  (1 / Enter)",
            "  How to Play  (2)",
            "  Settings  (3)",
            "  Quit  (4)"
        };

        for (int i = 0; i < 4; i++)
        {
            RectF buttonRect = getMainMenuButtonRect(windowWidth, windowHeight, i);
            bool hovered = buttonRect.contains(gameState.mouseX, gameState.mouseY);
            drawMenuButton(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height,
                           labels[i], gameState.menuSelection == i, hovered);
        }

        HUD::drawPanel(panelX + 40.0f, panelY + 30.0f, panelW - 80.0f, 90.0f,
                       0.45f, 0.68f, 0.96f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, panelY + 94.0f,
                                         "Best Score:", 0.98f, 0.92f, 0.58f);

        char scoreText[32];
        std::snprintf(scoreText, sizeof(scoreText), "%d", highScores.getBestScore());
        TextRenderer::drawTextWithShadow(panelX + 180.0f, panelY + 94.0f,
                                         scoreText, 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(panelX + 60.0f, panelY + 60.0f,
                                         "Arrow keys or 1-4 to select, Enter or click to confirm",
                                         0.74f, 0.84f, 1.0f);
    }

    void drawPauseMenu(int windowWidth, int windowHeight,
                       const GameStateData& gameState)
    {
        float panelW = 400.0f;
        float panelH = 320.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;

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

        const char* labels[4] = {
            "  Resume  (P / ESC)",
            "  Restart Level  (R)",
            "  Main Menu  (M)",
            "  Quit  (Q)"
        };

        for (int i = 0; i < 4; i++)
        {
            RectF buttonRect = getPauseMenuButtonRect(windowWidth, windowHeight, i);
            bool hovered = buttonRect.contains(gameState.mouseX, gameState.mouseY);
            drawMenuButton(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height,
                           labels[i], false, hovered);
        }

        TextRenderer::drawTextWithShadow(panelX + 20.0f, panelY + 14.0f,
                                         "Click a button or use keyboard",
                                         0.60f, 0.70f, 0.90f);
    }

    void drawHowToPlay(int windowWidth, int windowHeight,
                       const GameStateData& gameState)
    {
        float panelW = 760.0f;
        float panelH = 620.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float cardX = panelX + 40.0f;
        float cardW = panelW - 80.0f;
        float objectiveH = 100.0f;
        float controlsH = 150.0f;
        float hazardsH = 120.0f;
        float objectiveY = panelY + 448.0f;
        float controlsY = objectiveY - controlsH - 24.0f;
        float hazardsY = controlsY - hazardsH - 24.0f;

        Renderer::drawFilledRect(0.0f, 0.0f, static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight),
                                 Colors::BG_DARK_R, Colors::BG_DARK_G, Colors::BG_DARK_B, 1.0f);

        Renderer::drawFilledRect(panelX, panelY, panelW, panelH,
                                 0.02f, 0.04f, 0.07f, 0.95f);
        Renderer::drawRectOutline(panelX, panelY, panelW, panelH, 2.5f,
                                  Colors::ACCENT_BLUE_R, Colors::ACCENT_BLUE_G,
                                  Colors::ACCENT_BLUE_B, 0.60f);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + panelH - 44.0f,
                                         "HOW TO PLAY",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);

        HUD::drawPanel(cardX, objectiveY, cardW, objectiveH,
                       0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, objectiveY + objectiveH - 38.0f,
                                         "OBJECTIVE", 0.58f, 0.90f, 0.98f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, objectiveY + 30.0f,
                                         "Find the golden key, then reach the exit flag!",
                                         0.92f, 0.95f, 1.0f);

        HUD::drawPanel(cardX, controlsY, cardW, controlsH,
                       0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, controlsY + controlsH - 38.0f,
                                         "CONTROLS", 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, controlsY + 84.0f,
                                         "WASD or Arrow Keys - Move one tile",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, controlsY + 52.0f,
                                         "P - Pause game",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, controlsY + 20.0f,
                                         "R - Restart current level",
                                         0.92f, 0.95f, 1.0f);

        HUD::drawPanel(cardX, hazardsY, cardW, hazardsH,
                       0.98f, 0.82f, 0.44f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, hazardsY + hazardsH - 38.0f,
                                         "HAZARDS", 0.98f, 0.82f, 0.44f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, hazardsY + 44.0f,
                                         "Red Traps - Lose 1 life and 125 points",
                                         0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(cardX + 24.0f, hazardsY + 12.0f,
                                         "Brown Obstacles - Block your path",
                                         0.92f, 0.95f, 1.0f);

        RectF backButton = getHowToPlayBackButtonRect(windowWidth, windowHeight);
        bool hovered = backButton.contains(gameState.mouseX, gameState.mouseY);
        drawMenuButton(backButton.x, backButton.y, backButton.width, backButton.height,
                       "  Back  (Enter / ESC)", false, hovered);

        TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 42.0f,
                                         "Use the button, Enter, or ESC to return",
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
    }

    void drawSettings(int windowWidth, int windowHeight,
                      const GameStateData& gameState)
    {
        float panelW = 580.0f;
        float panelH = 430.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        RectF nameField = getSettingsNameFieldRect(windowWidth, windowHeight);
        RectF primaryButton = getSettingsButtonRect(windowWidth, windowHeight, 0);
        RectF secondaryButton = getSettingsButtonRect(windowWidth, windowHeight, 1);

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

        HUD::drawPanel(nameField.x, nameField.y, nameField.width, nameField.height,
                       0.70f, 0.95f, 0.74f,
                       gameState.settingsEditingName ? 0.68f : 0.46f);
        Renderer::drawRectOutline(nameField.x, nameField.y, nameField.width, nameField.height,
                                  gameState.settingsEditingName ? 2.2f : 1.6f,
                                  0.40f, 0.95f, 0.60f,
                                  gameState.settingsEditingName ? 0.92f : 0.58f);

        TextRenderer::drawTextWithShadow(nameField.x + 24.0f, nameField.y + nameField.height - 48.0f,
                                         "Player Name", 0.70f, 0.95f, 0.74f);
        TextRenderer::drawTextWithShadow(nameField.x + 24.0f, nameField.y + nameField.height - 74.0f,
                                         "Used in the HUD and high score table",
                                         0.66f, 0.78f, 0.92f);

        int elapsed = glutGet(GLUT_ELAPSED_TIME);
        bool showCursor = gameState.settingsEditingName && ((elapsed / 500) % 2 == 0);
        char nameDisplay[Config::MAX_PLAYER_NAME_LENGTH + 3];
        std::snprintf(nameDisplay, sizeof(nameDisplay), "%s%s",
                      gameState.settingsDraftName,
                      showCursor ? "|" : "");
        TextRenderer::drawTextWithShadow(nameField.x + 24.0f, nameField.y + 28.0f,
                                         nameDisplay, 0.95f, 0.98f, 1.0f);

        const char* primaryLabel = gameState.settingsEditingName
            ? "  Save  (Enter)"
            : "  Edit  (E)";
        const char* secondaryLabel = gameState.settingsEditingName
            ? "  Cancel  (Esc)"
            : "  Back  (Esc)";

        drawMenuButton(primaryButton.x, primaryButton.y,
                       primaryButton.width, primaryButton.height,
                       primaryLabel, false,
                       primaryButton.contains(gameState.mouseX, gameState.mouseY));
        drawMenuButton(secondaryButton.x, secondaryButton.y,
                       secondaryButton.width, secondaryButton.height,
                       secondaryLabel, false,
                       secondaryButton.contains(gameState.mouseX, gameState.mouseY));

        if (gameState.settingsEditingName)
        {
            TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 120.0f,
                                             "Type freely. Backspace deletes. Save applies the new name.",
                                             0.72f, 0.80f, 0.92f);
            TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 88.0f,
                                             "Cancel restores the previous saved name.",
                                             Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                             Colors::TEXT_GOLD_B);
        }
        else
        {
            TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 120.0f,
                                             "Click Edit or press E to start renaming.",
                                             0.72f, 0.80f, 0.92f);
            TextRenderer::drawTextWithShadow(panelX + 40.0f, panelY + 88.0f,
                                             "Back returns without changing the saved name.",
                                             Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                             Colors::TEXT_GOLD_B);
        }
    }
}
