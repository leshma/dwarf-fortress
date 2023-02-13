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

class LevelManager
{
public:
    std::vector<std::shared_ptr<Object>> LoadedObjects;
    std::vector<std::shared_ptr<Room>> LoadedRooms;

    static const std::string QuickSaveFilePath;

    LevelManager(const std::string& levelFilePath)
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
    
    void SaveFile(const std::string& levelFilePath, std::shared_ptr<Environment> environment)
    {
        std::ofstream file;
        file.open(levelFilePath);

        Json::Value levelJson;

        // Player
        std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(environment->Objects[environment->PlayerPosition.Y][environment->PlayerPosition.X]);

        // Enemies
        std::vector<std::shared_ptr<Enemy>> enemies;
        enemies.reserve(environment->EnemyPositions.size());
        std::transform(environment->EnemyPositions.begin(), environment->EnemyPositions.end(), std::back_inserter(enemies), [environment](const auto& e)
        {
            return std::dynamic_pointer_cast<Enemy>(environment->Objects[e.Y][e.X]);
        });

        // Items
        std::vector<std::shared_ptr<Item>> items;
        for (const auto& objectRow : environment->Objects)
        {
            for (const auto object : objectRow)
            {
                if (object != nullptr && object->GetType() == TItem)
                {
                    items.push_back(std::dynamic_pointer_cast<Item>(object));
                }
            }
        }
        
        levelJson["Player"] = SavePlayer(player);
        levelJson["NonPlayableCharacters"]["Enemies"] = SaveEnemies(enemies);
        levelJson["Items"] = SaveItems(items);
        levelJson["Rooms"] = SaveRooms(LoadedRooms);
        
        file << levelJson;

        file.close();
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

        std::shared_ptr<Item> weapon;
        if (levelJson["Player"].isMember("Weapon"))
        {
            weapon = LoadItem(levelJson["Player"]["Weapon"]);
        }

        std::shared_ptr<Item> armor;
        if (levelJson["Player"].isMember("Armor"))
        {
            armor = LoadItem(levelJson["Player"]["Armor"]);
        }

        std::vector<std::shared_ptr<Item>> items;
        if (levelJson["Player"].isMember("Items"))
        {
            items = LoadItems(levelJson["Player"]["Items"]);
        }

        std::unique_ptr<Inventory> inventory(new Inventory());
        inventory->Weapon = weapon;
        inventory->Armor = armor;
        inventory->ContainedItems = items;
        
        LoadedObjects.emplace_back(new Player(playerPosition, playerHealth, std::move(inventory)));
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

    Json::Value SavePlayer(std::shared_ptr<Player> player)
    {
        Json::Value playerJson;
        
        playerJson["Position"] = SaveCoordinates(player->Position);
        playerJson["Health"] = player->Health;
        
        if (player->Inventory->Weapon != nullptr) 
            playerJson["Weapon"] = SaveItem(player->Inventory->Weapon);

        if (player->Inventory->Armor != nullptr) 
            playerJson["Armor"] = SaveItem(player->Inventory->Armor);

        playerJson["Items"] = SaveItems(player->Inventory->ContainedItems);

        return playerJson;
    }
    Json::Value SaveEnemies(std::vector<std::shared_ptr<Enemy>> enemies)
    {
        Json::Value enemiesArray(Json::arrayValue);

        for (auto enemy : enemies)
        {
            if (enemy != nullptr) enemiesArray.append(SaveEnemy(enemy));
        }

        return enemiesArray;
    }
    Json::Value SaveEnemy(std::shared_ptr<Enemy> enemy)
    {
        Json::Value enemyJson;

        enemyJson["Position"] = SaveCoordinates(enemy->Position);
        enemyJson["Health"] = enemy->Health;

        if (enemy->Weapon != nullptr)
            enemyJson["Weapon"] = SaveItem(enemy->Weapon);

        if (enemy->Armor != nullptr)
            enemyJson["Armor"] = SaveItem(enemy->Armor);

        if (!enemy->DroppableItems.empty())
            enemyJson["DroppableItems"] = SaveItems(enemy->DroppableItems);

        return enemyJson;
    }
    Json::Value SaveRooms(std::vector<std::shared_ptr<Room>> rooms)
    {
        Json::Value roomsArray(Json::arrayValue);

        for (auto room : rooms)
        {
            if (room != nullptr) roomsArray.append(SaveRoom(room));
        }

        return roomsArray;
    }
    Json::Value SaveRoom(std::shared_ptr<Room> room)
    {
        Json::Value roomJson;

        roomJson["From"] = SaveCoordinates(room->From);
        roomJson["To"] = SaveCoordinates(room->To);
        roomJson["EntryPoint"] = room->EntryPoint;

        return roomJson;
    }
    Json::Value SaveItems(std::vector<std::shared_ptr<Item>> items)
    {
        Json::Value itemsArray(Json::arrayValue);
        
        for (auto item : items)
        {
            if (item != nullptr) itemsArray.append(SaveItem(item));
        }

        return itemsArray;
    }
    Json::Value SaveItem(std::shared_ptr<Item> item)
    {
        Json::Value itemJson;

        itemJson["PersistanceType"] = item->PersistanceType;
        itemJson["ItemType"] = item->ItemType;
        itemJson["Position"] = SaveCoordinates(item->Position);
        itemJson["Statistics"] = SaveStatistics(item->Statistics);

        return itemJson;
    }
    Json::Value SaveStatistics(std::vector<std::shared_ptr<Statistic>> statistics)
    {
        Json::Value statisticsJson(Json::arrayValue);

        for (auto statistic : statistics)
        {
            statisticsJson.append(SaveStatistic(statistic));
        }

        return statisticsJson;
    }
    Json::Value SaveStatistic(std::shared_ptr<Statistic> statistic)
    {
        Json::Value statisticJson;

        statisticJson["Type"] = statistic->Type;
        statisticJson["Value"] = statistic->Value;

        return statisticJson;
    }
    Json::Value SaveCoordinates(Coordinates position)
    {
        Json::Value positionArray(Json::arrayValue);
        
        positionArray.append(position.X);
        positionArray.append(position.Y);

        return positionArray;
    }
};
