#pragma once
#include <fstream>
#include <vector>
#include <experimental/filesystem>

#include "../Externals/json/json.h"
#include "../Models/EntryPoint.h"
#include "../Models/Object.h"
#include "../Models/Player.h"
#include "../Models/Room.h"

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

        if (!levelJson.isMember("Player"))
            throw "The level file doesn't contain information about the player!";

        Coordinates playerPosition = {
            levelJson["Player"]["Position"][0].asInt(),
            levelJson["Player"]["Position"][1].asInt(),
        };
        int playerHealth = levelJson["Player"]["Health"].asInt();
        LoadedObjects.emplace_back(new Player(playerPosition, playerHealth, std::make_unique<Inventory>()));

        if (levelJson.isMember("Rooms"))
        {
            auto roomsJson = levelJson["Rooms"];
            
            LoadedRooms.reserve(roomsJson.size());
            std::transform(roomsJson.begin(), roomsJson.end(), std::back_inserter(LoadedRooms), [](const auto& e)
            {
                Coordinates from =   { e["From"][0].asInt(), e["From"][1].asInt() };
                Coordinates to =     { e["To"][0].asInt(),   e["To"][1].asInt() };
                EntryPoint entryPoint = static_cast<EntryPoint>(e["EntryPoint"].asInt());
                return std::make_shared<Room>(from, to, entryPoint);
            });
        }
    }
};
