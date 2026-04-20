#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "../core/config.h"

struct HighScoreEntry
{
    char name[Config::MAX_PLAYER_NAME_LENGTH + 1];
    int score;
};

class HighScoreManager
{
public:
    HighScoreManager();

    // Load/save with error handling
    // Returns true if load/save successful, false on file error or parse failure
    bool load();
    bool save();
    void add(const char *playerName, int score);

    int getCount() const { return count; }
    const HighScoreEntry &getEntry(int index) const { return entries[index]; }
    int getBestScore() const;

private:
    HighScoreEntry entries[Config::MAX_HIGH_SCORES];
    int count;
};

#endif // HIGHSCORE_H
