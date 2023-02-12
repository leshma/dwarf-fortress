#pragma once
#include <fstream>
#include <vector>
#include <experimental/filesystem>

#include "../Externals/json/json.h"
#include "../Models/Enemy.h"
#include "../Models/Direction.h"
#include "../Models/Object.h"
#include "../Models/Player.h"
#include "../Models/Room.h"
#include "../Models/Statistics/Statistic.h"

class LevelLoader
{
public:
    std::vector<std::shared_ptr<Object>> LoadedObjects;
    std::vector<std::shared_ptr<Room>> LoadedRooms;

    LevelLoader(const std::string& levelFilePath)
    {
        LoadFile(levelFilePath);
    }
    
    void LoadFile(const std::string& levelFilePath)
    {
        if (!std::experimental::filesystem::exists(levelFilePath))
            throw "The level file doesn't exist or the path is wrong.";

        if (levelFilePath.substr(levelFilePath.find_last_of(".") + 1) != "json")
            throw "The level file isn't in a JSON format.";

        std::ifstream levelFile(levelFilePath, std::ifstream::binary);
        Json::Value levelJson;

        levelFile >> levelJson;

        LoadPlayer(levelJson);
        LoadRooms(levelJson);

        if (levelJson.isMember("Items"))
        {
            const auto items = LoadItems(levelJson["Items"]);
            LoadedObjects.insert(LoadedObjects.begin(), items.begin(), items.end());
        }

        LoadNonPlayableCharacters(levelJson);
    }
private:
    void LoadPlayer(Json::Value levelJson)
    {
        if (!levelJson.isMember("Player"))
            throw "The level file doesn't contain information about the player!";

        Coordinates playerPosition = {
            levelJson["Player"]["Position"][0].asInt(),
            levelJson["Player"]["Position"][1].asInt(),
        };
        int playerHealth = levelJson["Player"]["Health"].asInt();
        LoadedObjects.emplace_back(new Player(playerPosition, playerHealth, std::make_unique<Inventory>()));
    }

    void LoadRooms(Json::Value levelJson)
    {
        if (levelJson.isMember("Rooms"))
        {
            auto roomsJson = levelJson["Rooms"];
            
            LoadedRooms.reserve(roomsJson.size());
            std::transform(roomsJson.begin(), roomsJson.end(), std::back_inserter(LoadedRooms), [](const auto& e)
            {
                Coordinates from =   { e["From"][0].asInt(), e["From"][1].asInt() };
                Coordinates to =     { e["To"][0].asInt(),   e["To"][1].asInt() };
                Direction entryPoint = static_cast<Direction>(e["EntryPoint"].asInt());
                return std::make_shared<Room>(from, to, entryPoint);
            });
        }
    }

    std::vector<std::shared_ptr<Item>> LoadItems(Json::Value itemsJson)
    {
        std::vector<std::shared_ptr<Item>> items;
        std::transform(itemsJson.begin(), itemsJson.end(), std::back_inserter(items), [this](const auto& e)
        {
            return LoadItem(e);
        });

        return items;
    }

    std::shared_ptr<Item> LoadItem(Json::Value itemJson)
    {
        Coordinates position = { itemJson["Position"][0].asInt(), itemJson["Position"][1].asInt() };
        PersistanceType persistanceType = static_cast<PersistanceType>(itemJson["PersistanceType"].asInt());
        ItemType itemType = static_cast<ItemType>(itemJson["ItemType"].asInt());
            
        auto statisticsJson = itemJson["Statistics"];
        std::vector<std::shared_ptr<Statistic>> statistics;

        std::transform(statisticsJson.begin(), statisticsJson.end(), std::back_inserter(statistics), [](const auto& s)
        {
            StatisticType type = static_cast<StatisticType>(s["Type"].asInt());
            double value = s["Value"].asDouble();
            return std::make_shared<Statistic>(type, value);
        });
            
        return std::make_shared<Item>(position, statistics, persistanceType, itemType);
    }

    void LoadNonPlayableCharacters(Json::Value levelJson)
    {
        if (levelJson.isMember("NonPlayableCharacters"))
        {
            LoadEnemies(levelJson["NonPlayableCharacters"]);
        }
    }

    void LoadEnemies(Json::Value levelJson)
    {
        if (levelJson.isMember("Enemies"))
        {
            auto enemiesJson = levelJson["Enemies"];

            std::vector<std::shared_ptr<Enemy>> enemies;
            std::transform(enemiesJson.begin(), enemiesJson.end(), std::back_inserter(enemies), [this](const auto& e)
            {
                Coordinates position = { e["Position"][0].asInt(), e["Position"][1].asInt() };
                int health = e["Health"].asInt();
                std::shared_ptr<Item> weapon, armor;
                std::vector<std::shared_ptr<Item>> droppableItems;

                if (e.isMember("Weapon"))
                {
                    weapon = LoadItem(e["Weapon"]);
                }

                if (e.isMember("Armor"))
                {
                    armor = LoadItem(e["Armor"]);
                }

                if (e.isMember("DroppableItems"))
                {
                    droppableItems = LoadItems(e["DroppableItems"]);
                }
                
                return std::make_shared<Enemy>(position, health, weapon, armor, droppableItems);
            });

            LoadedObjects.insert(LoadedObjects.begin(), enemies.begin(), enemies.end());
        }
    }
};
