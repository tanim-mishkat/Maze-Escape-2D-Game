#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "config.h"

// Manages player settings persistence (name, preferences)
class SettingsManager
{
public:
    SettingsManager();

    // Load settings from file (or use defaults if file doesn't exist)
    void load();

    // Save settings to file
    void save();

    // Player name management
    const char *getPlayerName() const { return playerName; }
    void setPlayerName(const char *name);

    // Draft name management (for editing in menu)
    const char *getDraftName() const { return draftName; }
    void setDraftName(const char *name);
    void resetDraftName();

    // Append/remove character from draft name (for text input)
    void appendCharacterToDraftName(unsigned char key);
    void removeLastCharacterFromDraftName();

    // Copy draft to actual name (confirm edit)
    void confirmDraftName();

private:
    char playerName[Config::MAX_PLAYER_NAME_LENGTH + 1];
    char draftName[Config::MAX_PLAYER_NAME_LENGTH + 1];

    // Helper to ensure name is valid
    void ensurePlayerNameValid();
};

#endif // SETTINGS_MANAGER_H
