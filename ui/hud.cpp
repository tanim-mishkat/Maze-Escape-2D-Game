#include "hud.h"
#include "../render/renderer.h"
#include "../render/text.h"
#include "../render/colors.h"
#include <cstdio>

namespace HUD
{
    void drawPanel(float x, float y, float width, float height,
                  float accentR, float accentG, float accentB, float alpha)
    {
        Renderer::drawFilledRect(x, y, width, height, 0.03f, 0.05f, 0.08f, alpha);
        Renderer::drawFilledRect(x + 4.0f, y + height - 8.0f, width - 8.0f, 4.0f,
                                accentR, accentG, accentB, 0.96f);
        Renderer::drawRectOutline(x, y, width, height, 1.5f, accentR, accentG, accentB, 0.34f);
    }

    void drawInfoCard(float x, float y, float width, float height,
                     const char* label, const char* value,
                     float accentR, float accentG, float accentB)
    {
        Renderer::drawFilledRect(x, y, width, height, 0.03f, 0.05f, 0.08f, 0.82f);
        Renderer::drawFilledRect(x + 4.0f, y + height - 8.0f, width - 8.0f, 4.0f,
                                accentR, accentG, accentB, 0.92f);
        Renderer::drawRectOutline(x, y, width, height, 1.2f, accentR, accentG, accentB, 0.38f);

        TextRenderer::drawTextWithShadow(x + 12.0f, y + height - 22.0f, label,
                                        accentR, accentG, accentB);
        TextRenderer::drawTextWithShadow(x + 12.0f, y + 18.0f, value, 0.95f, 0.98f, 1.0f);
    }

    void drawHud(int windowWidth, int windowHeight, const GameStateData& gameState,
                const Level& level, const Timer& timer, const HighScoreManager& highScores)
    {
        char timerText[32];
        char levelText[64];
        char scoreText[32];
        char livesText[32];
        char bestText[32];
        const char* keyText = gameState.hasKey ? "YES" : "NO";

        std::snprintf(timerText, sizeof(timerText), "%.1fs", timer.getElapsedSeconds());
        
        if (level.isProcedural())
        {
            std::snprintf(levelText, sizeof(levelText), "Level %d/%d",
                         level.getIndex() + 1, Config::TOTAL_LEVELS);
        }
        else
        {
            std::snprintf(levelText, sizeof(levelText), "%s %d/%d",
                         level.getDefinition().name, level.getIndex() + 1, Config::TOTAL_LEVELS);
        }
        
        std::snprintf(scoreText, sizeof(scoreText), "%d", gameState.score);
        std::snprintf(livesText, sizeof(livesText), "%d / %d", gameState.lives, Config::MAX_LIVES);
        std::snprintf(bestText, sizeof(bestText), "%d", highScores.getBestScore());

        float panelX = Renderer::getSidebarX(windowWidth);
        float panelY = Renderer::getSidebarY(windowHeight);
        float innerX = panelX + 16.0f;
        float cardWidth = Config::SIDE_PANEL_WIDTH - 32.0f;
        float halfWidth = (cardWidth - 10.0f) / 2.0f;

        drawPanel(panelX, panelY, Config::SIDE_PANEL_WIDTH, Config::BOARD_HEIGHT,
                 0.52f, 0.74f, 1.0f);
        TextRenderer::drawTextWithShadow(innerX, panelY + Config::BOARD_HEIGHT - 30.0f,
                                        "Run Panel", 0.98f, 0.92f, 0.58f);
        TextRenderer::drawTextWithShadow(innerX, panelY + Config::BOARD_HEIGHT - 56.0f,
                                        "Find the key, reach the exit!",
                                        0.82f, 0.92f, 1.0f);

        drawInfoCard(innerX, panelY + Config::BOARD_HEIGHT - 140.0f, cardWidth, 72.0f,
                    "Player", gameState.playerName, 0.58f, 0.90f, 0.98f);
        drawInfoCard(innerX, panelY + Config::BOARD_HEIGHT - 224.0f, halfWidth, 66.0f,
                    "Stage", levelText, 0.72f, 0.84f, 1.0f);
        drawInfoCard(innerX + halfWidth + 10.0f, panelY + Config::BOARD_HEIGHT - 224.0f,
                    halfWidth, 66.0f, "Timer", timerText, 0.96f, 0.82f, 0.44f);
        drawInfoCard(innerX, panelY + Config::BOARD_HEIGHT - 302.0f, halfWidth, 66.0f,
                    "Score", scoreText, 0.70f, 0.95f, 0.74f);
        drawInfoCard(innerX + halfWidth + 10.0f, panelY + Config::BOARD_HEIGHT - 302.0f,
                    halfWidth, 66.0f, "Lives", livesText, 1.0f, 0.78f, 0.72f);
        drawInfoCard(innerX, panelY + Config::BOARD_HEIGHT - 380.0f, halfWidth, 66.0f,
                    "Key", keyText, 1.0f, 0.84f, 0.0f);
        drawInfoCard(innerX + halfWidth + 10.0f, panelY + Config::BOARD_HEIGHT - 380.0f,
                    halfWidth, 66.0f, "Best", bestText, 0.98f, 0.92f, 0.58f);

        drawPanel(innerX, panelY + 18.0f, cardWidth, 160.0f, 0.84f, 0.92f, 1.0f, 0.78f);
        TextRenderer::drawTextWithShadow(innerX + 14.0f, panelY + 146.0f, "Controls",
                                        0.84f, 0.92f, 1.0f);
        TextRenderer::drawTextWithShadow(innerX + 14.0f, panelY + 118.0f,
                                        "W A S D  move one tile", 0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(innerX + 14.0f, panelY + 92.0f,
                                        "P        pause game", 0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(innerX + 14.0f, panelY + 66.0f,
                                        "R        restart level", 0.92f, 0.95f, 1.0f);
        TextRenderer::drawTextWithShadow(innerX + 14.0f, panelY + 40.0f,
                                        "Esc      quit", 0.92f, 0.95f, 1.0f);
    }
}
