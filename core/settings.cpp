#include "settings.h"
#include <fstream>
#include <cstring>

SettingsManager::SettingsManager()
{
    playerName[0] = '\0';
    draftName[0] = '\0';
    ensurePlayerNameValid();
}

void SettingsManager::load()
{
    std::ifstream file(Config::SETTINGS_FILE);
    std::string line;

    // Initialize defaults first
    ensurePlayerNameValid();

    if (!file.is_open())
    {
        // File doesn't exist - use defaults
        resetDraftName();
        return;
    }

    // Parse settings file
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (line.find("NAME=") == 0 && line.length() > 5)
        {
            // Extract player name (everything after "NAME=")
            std::string name = line.substr(5);
            setPlayerName(name.c_str());
        }
    }

    file.close();
    ensurePlayerNameValid();
    resetDraftName();
}

void SettingsManager::save()
{
    std::ofstream file(Config::SETTINGS_FILE);

    if (!file.is_open())
    {
        // Failed to open settings file
        return;
    }

    file << "NAME=" << playerName << "\n";

    if (!file.good())
    {
        file.close();
        return;
    }

    file.close();
}

void SettingsManager::setPlayerName(const char *name)
{
    if (!name || name[0] == '\0')
    {
        playerName[0] = '\0';
        ensurePlayerNameValid();
        return;
    }

    std::strncpy(playerName, name, Config::MAX_PLAYER_NAME_LENGTH);
    playerName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
    ensurePlayerNameValid();
}

void SettingsManager::setDraftName(const char *name)
{
    if (!name)
    {
        draftName[0] = '\0';
        return;
    }

    std::strncpy(draftName, name, Config::MAX_PLAYER_NAME_LENGTH);
    draftName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
}

void SettingsManager::resetDraftName()
{
    // Copy current player name to draft
    std::strncpy(draftName, playerName, Config::MAX_PLAYER_NAME_LENGTH);
    draftName[Config::MAX_PLAYER_NAME_LENGTH] = '\0';
}

void SettingsManager::appendCharacterToDraftName(unsigned char key)
{
    std::size_t length = std::strlen(draftName);

    // Validate: printable char, not pipe (used as separator), not space at start, room in buffer
    if (key < 32 || key > 126 || key == '|' || length >= Config::MAX_PLAYER_NAME_LENGTH)
    {
        return;
    }

    if (key == ' ' && length == 0)
    {
        return; // Don't start name with space
    }

    draftName[length] = static_cast<char>(key);
    draftName[length + 1] = '\0';
}

void SettingsManager::removeLastCharacterFromDraftName()
{
    std::size_t length = std::strlen(draftName);

    if (length > 0)
    {
        draftName[length - 1] = '\0';
    }
}

void SettingsManager::confirmDraftName()
{
    setPlayerName(draftName);
    save();
}

void SettingsManager::ensurePlayerNameValid()
{
    if (playerName[0] == '\0')
    {
        std::strcpy(playerName, "PLAYER");
    }
}
