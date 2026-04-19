#include "overlay.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include "../data/leveldata.h"
#include <GL/freeglut.h>
#include <cstdio>

namespace Overlay
{
    static bool inRect(int px, int py, float rx, float ry, float rw, float rh)
    {
        return px >= static_cast<int>(rx) && px <= static_cast<int>(rx + rw)
            && py >= static_cast<int>(ry) && py <= static_cast<int>(ry + rh);
    }

    static void drawButton(float x, float y, float w, float h,
                           const char* label, bool hovered)
    {
        float alpha = hovered ? 0.92f : 0.72f;
        float lineW = hovered ? 2.2f : 1.4f;
        float accentA = hovered ? 0.90f : 0.55f;

        Renderer::drawFilledRect(x, y, w, h, 0.04f, 0.07f, 0.11f, alpha);
        Renderer::drawRectOutline(x, y, w, h, lineW,
                                  0.96f, 0.84f, 0.30f, accentA);
        float tw = static_cast<float>(TextRenderer::getTextWidth(label));
        TextRenderer::drawTextWithShadow(x + (w - tw) / 2.0f, y + (h - 18.0f) / 2.0f + 2.0f,
                                         label,
                                         Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                         Colors::TEXT_GOLD_B);
    }

    static void drawOverlayBox(int windowWidth, int windowHeight, float width, float height)
    {
        float x = (windowWidth - width) / 2.0f;
        float y = (windowHeight - height) / 2.0f;

        Renderer::drawFilledRect(x, y, width, height, 0.03f, 0.05f, 0.08f, 0.92f);
        Renderer::drawRectOutline(x, y, width, height, 2.2f, 0.96f, 0.84f, 0.30f, 0.82f);
    }

    void drawLevelClearOverlay(int windowWidth, int windowHeight,
                               const GameStateData& gameState,
                               const Level& level)
    {
        char title[64];
        char bonusText[64];

        if (level.isProcedural())
            std::snprintf(title, sizeof(title), "Level %d Complete!", level.getIndex() + 1);
        else
            std::snprintf(title, sizeof(title), "%s Complete!", level.getDefinition().name);

        std::snprintf(bonusText, sizeof(bonusText), "Level Bonus: +%d", gameState.levelClearBonus);

        float panelW = 440.0f, panelH = 170.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;

        drawOverlayBox(windowWidth, windowHeight, panelW, panelH);

        float titleW = static_cast<float>(TextRenderer::getTextWidth(title));
        TextRenderer::drawTextWithShadow(panelX + (panelW - titleW) / 2.0f,
                                        panelY + 128.0f, title,
                                        Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                        Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 36.0f, panelY + 90.0f,
                                        bonusText, 0.70f, 0.95f, 0.74f);

        // Next-level button
        float btnW = 220.0f, btnH = 36.0f;
        float btnX = panelX + (panelW - btnW) / 2.0f;
        float btnY = panelY + 24.0f;
        bool hov = inRect(gameState.mouseX, gameState.mouseY, btnX, btnY, btnW, btnH);
        const char* nextLabel = (level.getIndex() + 1 < Config::TOTAL_LEVELS)
                                 ? "Next Level  (N / Enter)"
                                 : "Continue  (N / Enter)";
        drawButton(btnX, btnY, btnW, btnH, nextLabel, hov);
    }

    static void drawHighScoreList(float x, float y, float width,
                                  const HighScoreManager& highScores)
    {
        if (highScores.getCount() == 0)
        {
            Renderer::drawFilledRect(x, y - 20.0f, width, 26.0f, 0.05f, 0.07f, 0.10f, 0.68f);
            TextRenderer::drawTextWithShadow(x + 12.0f, y, "No high scores yet",
                                            0.88f, 0.92f, 1.0f);
            return;
        }

        for (int i = 0; i < highScores.getCount(); i++)
        {
            char rankText[8];
            char scoreText[20];
            float rowY = y - i * 30.0f;
            float rowTop = rowY - 18.0f;

            std::snprintf(rankText, sizeof(rankText), "%d.", i + 1);
            std::snprintf(scoreText, sizeof(scoreText), "%d", highScores.getEntry(i).score);

            Renderer::drawFilledRect(x, rowTop, width, 24.0f, 0.05f, 0.07f, 0.10f, 0.74f);
            Renderer::drawRectOutline(x, rowTop, width, 24.0f, 1.0f,
                                      0.40f, 0.62f, 0.90f, 0.18f);
            TextRenderer::drawTextWithShadow(x + 10.0f, rowY, rankText,
                                            0.96f, 0.90f, 0.56f);
            TextRenderer::drawTextWithShadow(x + 38.0f, rowY, highScores.getEntry(i).name,
                                            0.88f, 0.92f, 1.0f);
            TextRenderer::drawTextWithShadow(
                x + width - TextRenderer::getTextWidth(scoreText) - 12.0f,
                rowY, scoreText, 0.70f, 0.95f, 0.74f);
        }
    }

    void drawEndOverlay(int windowWidth, int windowHeight,
                       const char* title, const char* subtitle,
                       const GameStateData& gameState,
                       const HighScoreManager& highScores)
    {
        char scoreText[48];
        float panelW = 480.0f, panelH = 360.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;

        std::snprintf(scoreText, sizeof(scoreText), "Final Score: %d", gameState.score);

        drawOverlayBox(windowWidth, windowHeight, panelW, panelH);

        float titleW = static_cast<float>(TextRenderer::getTextWidth(title));
        TextRenderer::drawTextWithShadow(panelX + (panelW - titleW) / 2.0f,
                                        panelY + 316.0f, title,
                                        Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                        Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 282.0f,
                                        subtitle, 0.86f, 0.98f, 0.82f);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 252.0f,
                                        scoreText, 0.90f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 218.0f,
                                        "High Scores", 0.96f, 0.93f, 0.70f);
        drawHighScoreList(panelX + 30.0f, panelY + 190.0f, panelW - 60.0f, highScores);

        // Action buttons
        float btnW = 170.0f, btnH = 32.0f;
        float btnY = panelY + 16.0f;
        bool hovReplay = inRect(gameState.mouseX, gameState.mouseY,
                                panelX + 28.0f, btnY, btnW, btnH);
        bool hovMenu   = inRect(gameState.mouseX, gameState.mouseY,
                                panelX + 212.0f, btnY, btnW, btnH);
        bool hovQuit   = inRect(gameState.mouseX, gameState.mouseY,
                                panelX + 396.0f - btnW, btnY, btnW - 40.0f, btnH);

        drawButton(panelX + 28.0f,  btnY, btnW, btnH, "Replay  (R)", hovReplay);
        drawButton(panelX + 212.0f, btnY, btnW, btnH, "Menu  (M)",   hovMenu);
        drawButton(panelX + 380.0f, btnY, btnW - 40.0f, btnH, "Quit  (Q)", hovQuit);
    }
}
