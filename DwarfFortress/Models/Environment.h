#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>
#include <boost/signals2.hpp>

#include "Coordinates.h"
#include "Door.h"
#include "Enemy.h"
#include "Object.h"
#include "Player.h"
#include "Room.h"
#include "Wall.h"

// This class is used to process and gather data about the environment.
class Environment
{
public:
    std::vector<std::vector<std::shared_ptr<Object>>> Objects;
    Coordinates PlayerPosition;
    std::vector<Coordinates> EnemyPositions;

    boost::signals2::signal<void ()> SignalEnvironmentChanged;

    Environment(Coordinates dimensions, std::vector<std::shared_ptr<Object>> environmentObjects,
        std::vector<std::shared_ptr<Room>> environmentRooms)
    {
        _dimensions = dimensions;
        
        Objects = std::vector<std::vector<std::shared_ptr<Object>>>(_dimensions.Y, std::vector<std::shared_ptr<Object>>(_dimensions.X));
        for (int y = 0; y < Objects.size(); ++y)
            for (int x = 0; x < Objects[y].size(); ++x)
            {
                Objects[y][x].reset(new Object({x, y}, TNothing));
            }
        
        for (auto environmentObject : environmentObjects)
        {
            switch(environmentObject->GetType())
            {
            case TPlayer:
                PlayerPosition = environmentObject->Position;
                break;
            case TEnemy:
                EnemyPositions.emplace_back(environmentObject->Position);
                break;
            }
            Objects[environmentObject->Position.Y][environmentObject->Position.X] = environmentObject;
        }

        for (const auto environmentRoom : environmentRooms)
        {
            // bx by -> ex by
            // ex by -> ex ey
            // ex ey -> bx ey
            // bx ey -> bx by
            const std::vector<int> directions[4] = {
                {environmentRoom->From.X,   environmentRoom->From.Y, environmentRoom->To.X,   environmentRoom->From.Y, 1,  0, environmentRoom->EntryPoint == Top},
                {environmentRoom->To.X,     environmentRoom->From.Y, environmentRoom->To.X,   environmentRoom->To.Y,   0,  1, environmentRoom->EntryPoint == Right},
                {environmentRoom->From.X,   environmentRoom->To.Y,   environmentRoom->To.X,   environmentRoom->To.Y,   1,  0, environmentRoom->EntryPoint == Bottom},
                {environmentRoom->From.X,   environmentRoom->From.Y, environmentRoom->From.X, environmentRoom->To.Y,   0,  1, environmentRoom->EntryPoint == Left},
            };

            for (const auto& direction : directions)
            {
                Coordinates entryPointPosition = {-1, -1};
                if (direction[6])
                {
                    entryPointPosition.X = rand() % (direction[2] - direction[0] + 1);
                    entryPointPosition.Y = rand() % (direction[3] - direction[1] + 1);
                }
                
                for (int x = direction[0], y = direction[1];
                     x <= direction[2] && y <= direction[3];
                     x += direction[4], y+= direction[5])
                {
                    if ((entryPointPosition.X + direction[0]) == x && (entryPointPosition.Y + direction[1]) == y)
                    {
                        Objects[y][x] = std::make_shared<Door>(Coordinates({x, y}));
                    }
                    else
                    {
                        Objects[y][x] = std::make_shared<Wall>(Coordinates({x, y}));   
                    }
                } 
            }
        }
    }

    void MoveTick(WORD keyPressed)
    {
        switch (keyPressed)
        {
            case VK_LEFT:
                std::swap(Objects[PlayerPosition.Y][PlayerPosition.X - 1], Objects[PlayerPosition.Y][PlayerPosition.X]);
                PlayerPosition.X -= 1;
                break;
            case VK_RIGHT:
                std::swap(Objects[PlayerPosition.Y][PlayerPosition.X + 1], Objects[PlayerPosition.Y][PlayerPosition.X]);
                PlayerPosition.X += 1;
                break;
            case VK_UP:
                std::swap(Objects[PlayerPosition.Y - 1][PlayerPosition.X], Objects[PlayerPosition.Y][PlayerPosition.X]);
                PlayerPosition.Y -= 1;
                break;
            case VK_DOWN:
                std::swap(Objects[PlayerPosition.Y + 1][PlayerPosition.X], Objects[PlayerPosition.Y][PlayerPosition.X]);
                PlayerPosition.Y += 1;
                break;
        }

        SignalEnvironmentChanged();
    }
private:
    Coordinates _dimensions;
};
