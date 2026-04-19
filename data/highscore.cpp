#include "highscore.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

HighScoreManager::HighScoreManager() : count(0)
{
    for (int i = 0; i < Config::MAX_HIGH_SCORES; i++)
    {
        entries[i].name[0] = '\0';
        entries[i].score = 0;
    }
}

void HighScoreManager::load()
{
    FILE* file = std::fopen(Config::HIGH_SCORE_FILE, "r");
    char line[128];

    count = 0;

    if (file == nullptr)
    {
        return;
    }

    while (count < Config::MAX_HIGH_SCORES && std::fgets(line, sizeof(line), file) != nullptr)
    {
        HighScoreEntry entry = {};
        char* newLine = std::strchr(line, '\n');
        char* pipe = std::strrchr(line, '|');

        if (newLine != nullptr)
        {
            *newLine = '\0';
        }

        if (pipe != nullptr)
        {
            *pipe = '\0';
            std::snprintf(entry.name, Config::MAX_PLAYER_NAME_LENGTH + 1, "%s", 
                         line[0] == '\0' ? "PLAYER" : line);
            entry.score = std::atoi(pipe + 1);
        }
        else
        {
            std::snprintf(entry.name, Config::MAX_PLAYER_NAME_LENGTH + 1, "PLAYER");
            entry.score = std::atoi(line);
        }

        entries[count] = entry;
        count++;
    }

    std::fclose(file);
}

void HighScoreManager::save()
{
    FILE* file = std::fopen(Config::HIGH_SCORE_FILE, "w");

    if (file == nullptr)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
        std::fprintf(file, "%s|%d\n", entries[i].name, entries[i].score);
    }

    std::fclose(file);
}

void HighScoreManager::add(const char* playerName, int score)
{
    if (score < 0)
    {
        score = 0;
    }

    HighScoreEntry entry = {};
    int insertIndex = count;

    for (int i = 0; i < count; i++)
    {
        if (score > entries[i].score)
        {
            insertIndex = i;
            break;
        }
    }

    if (insertIndex >= Config::MAX_HIGH_SCORES)
    {
        save();
        return;
    }

    if (count < Config::MAX_HIGH_SCORES)
    {
        count++;
    }

    for (int i = count - 1; i > insertIndex; i--)
    {
        entries[i] = entries[i - 1];
    }

    std::snprintf(entry.name, Config::MAX_PLAYER_NAME_LENGTH + 1, "%s", playerName);
    entry.score = score;
    entries[insertIndex] = entry;
    save();
}

int HighScoreManager::getBestScore() const
{
    if (count == 0)
    {
        return 0;
    }
    return entries[0].score;
}
