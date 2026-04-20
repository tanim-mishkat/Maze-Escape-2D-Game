#include "overlay.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include "../data/leveldata.h"
#include <GL/freeglut.h>
#include <cstdio>

namespace Overlay
{
    namespace
    {
        void drawButton(float x, float y, float width, float height,
                        const char* label, bool hovered)
        {
            float alpha = hovered ? 0.92f : 0.72f;
            float lineWidth = hovered ? 2.2f : 1.4f;
            float accentAlpha = hovered ? 0.90f : 0.55f;

            Renderer::drawFilledRect(x, y, width, height, 0.04f, 0.07f, 0.11f, alpha);
            Renderer::drawRectOutline(x, y, width, height, lineWidth,
                                      0.96f, 0.84f, 0.30f, accentAlpha);

            float textWidth = static_cast<float>(TextRenderer::getTextWidth(label));
            TextRenderer::drawTextWithShadow(x + (width - textWidth) / 2.0f,
                                             y + (height - 18.0f) / 2.0f + 2.0f,
                                             label,
                                             Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                             Colors::TEXT_GOLD_B);
        }

        void drawOverlayBox(int windowWidth, int windowHeight, float width, float height)
        {
            float x = (windowWidth - width) / 2.0f;
            float y = (windowHeight - height) / 2.0f;

            Renderer::drawFilledRect(x, y, width, height, 0.03f, 0.05f, 0.08f, 0.92f);
            Renderer::drawRectOutline(x, y, width, height, 2.2f,
                                      0.96f, 0.84f, 0.30f, 0.82f);
        }

        void drawHighScoreList(float x, float y, float width,
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
    }

    RectF getLevelClearButtonRect(int windowWidth, int windowHeight)
    {
        float panelW = 440.0f;
        float panelH = 170.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float btnW = 220.0f;
        float btnH = 36.0f;

        return RectF(panelX + (panelW - btnW) / 2.0f, panelY + 24.0f, btnW, btnH);
    }

    bool isLevelClearContinueHit(int windowWidth, int windowHeight, int mouseX, int mouseY)
    {
        return getLevelClearButtonRect(windowWidth, windowHeight).contains(mouseX, mouseY);
    }

    RectF getEndOverlayButtonRect(int windowWidth, int windowHeight, EndOverlayAction action)
    {
        float panelW = 480.0f;
        float panelH = 360.0f;
        float panelX = (windowWidth - panelW) / 2.0f;
        float panelY = (windowHeight - panelH) / 2.0f;
        float innerWidth = panelW - 60.0f;
        float gap = 12.0f;
        float btnW = (innerWidth - gap * 2.0f) / 3.0f;
        float btnH = 32.0f;
        float btnY = panelY + 16.0f;
        float btnX = panelX + 30.0f + static_cast<int>(action) * (btnW + gap);

        return RectF(btnX, btnY, btnW, btnH);
    }

    EndOverlayAction hitTestEndOverlay(int windowWidth, int windowHeight, int mouseX, int mouseY)
    {
        for (int i = 0; i < 3; i++)
        {
            EndOverlayAction action = static_cast<EndOverlayAction>(i);
            if (getEndOverlayButtonRect(windowWidth, windowHeight, action).contains(mouseX, mouseY))
            {
                return action;
            }
        }

        return END_OVERLAY_ACTION_NONE;
    }

    void drawLevelClearOverlay(int windowWidth, int windowHeight,
                               const GameStateData& gameState,
                               const Level& level)
    {
        char title[64];
        char bonusText[64];

        std::snprintf(title, sizeof(title), "%s Complete!", level.getDefinition().name);
        std::snprintf(bonusText, sizeof(bonusText), "Level Bonus: +%d", gameState.levelClearBonus);

        float panelW = 440.0f;
        float panelH = 170.0f;
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

        RectF buttonRect = getLevelClearButtonRect(windowWidth, windowHeight);
        const char* nextLabel = (level.getIndex() + 1 < Config::TOTAL_LEVELS)
            ? "Next Level  (N / Enter)"
            : "Continue  (N / Enter)";

        drawButton(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height,
                   nextLabel, buttonRect.contains(gameState.mouseX, gameState.mouseY));
    }

    void drawEndOverlay(int windowWidth, int windowHeight,
                        const char* title, const char* subtitle,
                        const GameStateData& gameState,
                        const HighScoreManager& highScores)
    {
        char scoreText[48];
        float panelW = 480.0f;
        float panelH = 360.0f;
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

        RectF replayButton = getEndOverlayButtonRect(windowWidth, windowHeight, END_OVERLAY_ACTION_REPLAY);
        RectF menuButton = getEndOverlayButtonRect(windowWidth, windowHeight, END_OVERLAY_ACTION_MENU);
        RectF quitButton = getEndOverlayButtonRect(windowWidth, windowHeight, END_OVERLAY_ACTION_QUIT);

        drawButton(replayButton.x, replayButton.y, replayButton.width, replayButton.height,
                   "Replay  (R)", replayButton.contains(gameState.mouseX, gameState.mouseY));
        drawButton(menuButton.x, menuButton.y, menuButton.width, menuButton.height,
                   "Menu  (M)", menuButton.contains(gameState.mouseX, gameState.mouseY));
        drawButton(quitButton.x, quitButton.y, quitButton.width, quitButton.height,
                   "Quit  (Q)", quitButton.contains(gameState.mouseX, gameState.mouseY));
    }
}
