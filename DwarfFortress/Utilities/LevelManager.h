#pragma once
#include <fstream>
#include <vector>
#include <experimental/filesystem>

#include "../Externals/json/json.h"
#include "../Models/Enemy.h"
#include "../Models/Direction.h"
#include "../Models/Environment.h"
#include "../Models/Object.h"
#include "../Models/Player.h"
#include "../Models/Room.h"
#include "../Models/Statistics/Statistic.h"

/**
 * \brief A class used to Load and Save Levels for the game.
 */
class LevelManager
{
public:
    // Loaded objects from the level file.
    std::vector<std::shared_ptr<Object>> LoadedObjects;

    // Loaded rooms from the level file.
    std::vector<std::shared_ptr<Room>> LoadedRooms;

    // The quick-save file path.
    static const std::string QuickSaveFilePath;

    LevelManager(const std::string& levelFilePath);

    /**
     * \brief Loads the level from a file.
     * \param levelFilePath A file path towards a level file in a .json format.
     */
    void LoadFile(const std::string& levelFilePath);

    /**
     * \brief Saves the level to a file.
     * \param levelFilePath A path towards a saving .json file.
     * \param environment Environment that's going to be saved.
     */
    void SaveFile(const std::string& levelFilePath, std::shared_ptr<Environment> environment);
private:
    void LoadPlayer(Json::Value levelJson);
    void LoadRooms(Json::Value levelJson);
    std::vector<std::shared_ptr<Item>> LoadItems(Json::Value itemsJson);
    std::shared_ptr<Item> LoadItem(Json::Value itemJson);
    void LoadNonPlayableCharacters(Json::Value levelJson);
    void LoadEnemies(Json::Value levelJson);

    Json::Value SavePlayer(std::shared_ptr<Player> player);
    Json::Value SaveEnemies(std::vector<std::shared_ptr<Enemy>> enemies);
    Json::Value SaveEnemy(std::shared_ptr<Enemy> enemy);
    Json::Value SaveRooms(std::vector<std::shared_ptr<Room>> rooms);
    Json::Value SaveRoom(std::shared_ptr<Room> room);
    Json::Value SaveItems(std::vector<std::shared_ptr<Item>> items);
    Json::Value SaveItem(std::shared_ptr<Item> item);
    Json::Value SaveStatistics(std::vector<std::shared_ptr<Statistic>> statistics);
    Json::Value SaveStatistic(std::shared_ptr<Statistic> statistic);
    Json::Value SaveCoordinates(Coordinates position);
};
