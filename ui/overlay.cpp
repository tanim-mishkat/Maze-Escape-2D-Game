#include "overlay.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include "../data/leveldata.h"
#include <cstdio>

namespace Overlay
{
    static void drawOverlayBox(int windowWidth, int windowHeight, float width, float height)
    {
        float x = (windowWidth - width) / 2.0f;
        float y = (windowHeight - height) / 2.0f;

        Renderer::drawFilledRect(x, y, width, height, 0.03f, 0.05f, 0.08f, 0.88f);
        Renderer::drawRectOutline(x, y, width, height, 2.0f, 0.96f, 0.84f, 0.30f, 0.80f);
    }

    void drawLevelClearOverlay(int windowWidth, int windowHeight,
                               const GameStateData& gameState,
                               const Level& level)
    {
        char title[64];
        char bonusText[64];
        char promptText[64];

        if (level.isProcedural())
        {
            std::snprintf(title, sizeof(title), "Level %d Complete", level.getIndex() + 1);
        }
        else
        {
            std::snprintf(title, sizeof(title), "%s Complete", level.getDefinition().name);
        }
        
        std::snprintf(bonusText, sizeof(bonusText), "Level Bonus: %d", gameState.levelClearBonus);
        
        if (level.getIndex() + 1 < Config::TOTAL_LEVELS)
        {
            std::snprintf(promptText, sizeof(promptText), "Press N or Enter for next level");
        }
        else
        {
            std::snprintf(promptText, sizeof(promptText), "Press N or Enter to continue");
        }

        float panelWidth = 420.0f;
        float panelHeight = 150.0f;
        float panelX = (windowWidth - panelWidth) / 2.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        drawOverlayBox(windowWidth, windowHeight, panelWidth, panelHeight);
        TextRenderer::drawTextWithShadow(panelX + (panelWidth - TextRenderer::getTextWidth(title)) / 2.0f,
                                        panelY + 112.0f, title,
                                        Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                        Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 36.0f, panelY + 74.0f, bonusText,
                                        0.90f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 36.0f, panelY + 42.0f, promptText,
                                        0.86f, 0.98f, 0.82f);
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
            Renderer::drawRectOutline(x, rowTop, width, 24.0f, 1.0f, 0.40f, 0.62f, 0.90f, 0.18f);
            TextRenderer::drawTextWithShadow(x + 10.0f, rowY, rankText, 0.96f, 0.90f, 0.56f);
            TextRenderer::drawTextWithShadow(x + 38.0f, rowY, highScores.getEntry(i).name,
                                            0.88f, 0.92f, 1.0f);
            TextRenderer::drawTextWithShadow(x + width - TextRenderer::getTextWidth(scoreText) - 12.0f,
                                            rowY, scoreText, 0.70f, 0.95f, 0.74f);
        }
    }

    void drawEndOverlay(int windowWidth, int windowHeight,
                       const char* title, const char* subtitle,
                       const GameStateData& gameState,
                       const HighScoreManager& highScores)
    {
        char scoreText[48];
        const char* prompt = "Press R to replay or T for title";
        float panelWidth = 460.0f;
        float panelHeight = 340.0f;
        float panelX = (windowWidth - panelWidth) / 2.0f;
        float panelY = (windowHeight - panelHeight) / 2.0f;

        std::snprintf(scoreText, sizeof(scoreText), "Final Score: %d", gameState.score);

        drawOverlayBox(windowWidth, windowHeight, panelWidth, panelHeight);
        TextRenderer::drawTextWithShadow(panelX + (panelWidth - TextRenderer::getTextWidth(title)) / 2.0f,
                                        panelY + 298.0f, title,
                                        Colors::TEXT_GOLD_R, Colors::TEXT_GOLD_G,
                                        Colors::TEXT_GOLD_B);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 264.0f, subtitle,
                                        0.86f, 0.98f, 0.82f);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 234.0f, scoreText,
                                        0.90f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 204.0f, "High Scores",
                                        0.96f, 0.93f, 0.70f);
        drawHighScoreList(panelX + 30.0f, panelY + 174.0f, panelWidth - 60.0f, highScores);
        TextRenderer::drawTextWithShadow(panelX + 30.0f, panelY + 18.0f, prompt,
                                        0.90f, 0.95f, 1.0f);
    }
}
