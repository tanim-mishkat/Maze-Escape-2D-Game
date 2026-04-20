#include "highscore.h"
#include <fstream>
#include <sstream>
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

bool HighScoreManager::load()
{
    std::ifstream file(Config::HIGH_SCORE_FILE);
    std::string line;

    count = 0;

    if (!file.is_open())
    {
        // File doesn't exist or can't be opened - silently initialize empty
        return false;
    }

    while (count < Config::MAX_HIGH_SCORES && std::getline(file, line))
    {
        // Skip empty lines
        if (line.empty())
        {
            continue;
        }

        HighScoreEntry entry = {};
        std::size_t pipePos = line.find('|');

        if (pipePos != std::string::npos)
        {
            // Format: "NAME|SCORE"
            std::string nameStr = line.substr(0, pipePos);
            std::string scoreStr = line.substr(pipePos + 1);

            // Validate score string
            char *scoreEnd = nullptr;
            long scoreVal = std::strtol(scoreStr.c_str(), &scoreEnd, 10);

            // If no valid characters were parsed or score is out of range, skip entry
            if (scoreEnd == scoreStr.c_str() || scoreVal < 0 || scoreVal > 1000000)
            {
                continue;
            }

            // Copy name, truncate if too long
            std::strncpy(entry.name,
                         nameStr.empty() ? "PLAYER" : nameStr.c_str(),
                         Config::MAX_PLAYER_NAME_LENGTH);
            entry.name[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
            entry.score = static_cast<int>(scoreVal);
        }
        else
        {
            // No pipe separator found - invalid format, skip entry
            continue;
        }

        entries[count] = entry;
        count++;
    }

    file.close();
    return true; // Successfully loaded (even if file was empty)
}

bool HighScoreManager::save()
{
    std::ofstream file(Config::HIGH_SCORE_FILE);

    if (!file.is_open())
    {
        // Failed to open file for writing - silently fail
        // (alternative: log error to stderr)
        return false;
    }

    for (int i = 0; i < count; i++)
    {
        file << entries[i].name << "|" << entries[i].score << "\n";
    }

    if (!file.good())
    {
        // Write failed - file may be corrupted
        // (alternative: log error to stderr)
        file.close();
        return false;
    }

    file.close();
    return true; // Successfully saved
}

void HighScoreManager::add(const char *playerName, int score)
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
