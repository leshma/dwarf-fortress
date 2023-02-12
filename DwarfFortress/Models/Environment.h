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
#include "../Utilities/AdditionalVirtualKeyCodes.h"
#include "../ViewModels/EnvironmentChange.h"

// This class is used to process and gather data about the environment.
class Environment
{
public:
    std::vector<std::vector<std::shared_ptr<Object>>> Objects;
    Coordinates PlayerPosition;
    std::vector<Coordinates> EnemyPositions;

    boost::signals2::signal<void (std::vector<EnvironmentChange>)> SignalEnvironmentChanged;
    boost::signals2::signal<void ()> SignalStatusChanged;

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
        std::vector<EnvironmentChange> changes;
        
        int xMovement = 0, yMovement = 0;
        switch (keyPressed)
        {
            case VK_LEFT:
            case VK_A:
                xMovement = -1;
                break;
            case VK_RIGHT:
            case VK_D:
                xMovement = 1;
                break;
            case VK_UP:
            case VK_W:
                yMovement = -1;
                break;
            case VK_DOWN:
            case VK_S:
                yMovement = 1;
                break;
            case 0x31: // Key "1" from https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
                // Change / Equip weapon
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);

                    int i = 0;
                    for (; i < player->Inventory->ContainedItems.size(); ++i)
                    {
                        if (player->Inventory->ContainedItems[i]->ItemType == TWeapon)
                            break;
                    }

                    player->Inventory->ContainedItems.push_back(player->Inventory->Weapon);
                    player->Inventory->Weapon = player->Inventory->ContainedItems[i];
                    player->Inventory->ContainedItems.erase(player->Inventory->ContainedItems.begin() + i);

                    SignalStatusChanged();
                }
                break;
            case 0x32: // Key "2" from https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
                // Change / Equip armor
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);

                    int i = 0;
                    for (; i < player->Inventory->ContainedItems.size(); ++i)
                    {
                        if (player->Inventory->ContainedItems[i]->ItemType == TArmor)
                            break;
                    }

                    player->Inventory->ContainedItems.push_back(player->Inventory->Armor);
                    player->Inventory->Armor = player->Inventory->ContainedItems[i];
                    player->Inventory->ContainedItems.erase(player->Inventory->ContainedItems.begin() + i);

                    SignalStatusChanged();
                }
                break;
        }

        // Check if there's any movement...
        if (xMovement == 0 && yMovement == 0) return;
        
        const auto playerMoveChanges = MovePlayer(
            Objects[PlayerPosition.Y + yMovement][PlayerPosition.X + xMovement],
            Objects[PlayerPosition.Y][PlayerPosition.X],
            xMovement,
            yMovement
        );
        changes.insert(changes.end(), playerMoveChanges.begin(), playerMoveChanges.end());
        
        SignalEnvironmentChanged(changes);
        SignalStatusChanged();
    }

    std::vector<EnvironmentChange> MovePlayer(std::shared_ptr<Object>& object, std::shared_ptr<Object>& playerObject, int xMovement, int yMovement)
    {
        std::vector<EnvironmentChange> changes;
        
        bool moved = false;
        switch (object->GetType())
        {
            case TWall: return changes;
            case TNothing:
            case TDoor:
                changes.push_back(EnvironmentChange({object->Position.X, object->Position.Y}, {playerObject->Position.X, playerObject->Position.Y}));
                std::swap(object, playerObject);
                moved = true;
                break;
            case TItem:
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(playerObject);
                    std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(object);

                    if (item->PersistanceType == TEquippable)
                    {
                        switch (item->ItemType)
                        {
                        case TWeapon:
                            if (player->Inventory->Weapon == nullptr)
                            {
                                player->Inventory->Weapon = item;
                            }
                            else
                            {
                                player->Inventory->ContainedItems.push_back(item);
                            }
                            break;
                        case TArmor:
                            if (player->Inventory->Armor == nullptr)
                            {
                                player->Inventory->Armor = item;
                            }
                            else
                            {
                                player->Inventory->ContainedItems.push_back(item);
                            }
                            break;
                        default:
                            player->Inventory->ContainedItems.push_back(item);
                            break;
                        }
                    }
            
                    changes.push_back(EnvironmentChange({object->Position.X, object->Position.Y}, {playerObject->Position.X, playerObject->Position.Y}, true, false));
                    object.reset(new Object(object->Position, TNothing));
                    std::swap(object, playerObject);
            
                    moved = true;
                    break;
                }
            case TEnemy:
                // Fight enemy
                return changes;
        }

        if (moved)
        {
            object->Position.X += xMovement;
            object->Position.Y += yMovement;
            playerObject->Position.X -= xMovement;
            playerObject->Position.Y -= yMovement;
            
            PlayerPosition.X += xMovement;
            PlayerPosition.Y += yMovement;
        }

        return changes;
    }
private:
    Coordinates _dimensions;
};
