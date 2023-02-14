#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>
#include <boost/signals2.hpp>

#include "Combat.h"
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
    boost::signals2::signal<void (std::string)> SignalActionTextChanged;

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
                    entryPointPosition.X = (direction[2] - direction[0]) / 2;
                    entryPointPosition.Y = (direction[3] - direction[1]) / 2;
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
            case VK_1: // Key "1" from https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
                // Change / Equip weapon
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);

                    int i = 0;
                    for (; i < player->Inventory->ContainedItems.size(); ++i)
                    {
                        if (player->Inventory->ContainedItems[i]->ItemType == TWeapon)
                            break;
                    }

                    if (i >= player->Inventory->ContainedItems.size()) break;

                    player->Inventory->ContainedItems.push_back(player->Inventory->Weapon);
                    player->Inventory->Weapon = player->Inventory->ContainedItems[i];
                    player->Inventory->ContainedItems.erase(player->Inventory->ContainedItems.begin() + i);

                    SignalActionTextChanged("Weapon changed!");
                }
                break;
            case VK_2: // Key "2" from https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
                // Change / Equip armor
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);

                    int i = 0;
                    for (; i < player->Inventory->ContainedItems.size(); ++i)
                    {
                        if (player->Inventory->ContainedItems[i]->ItemType == TArmor)
                            break;
                    }

                    if (i >= player->Inventory->ContainedItems.size()) break;

                    player->Inventory->ContainedItems.push_back(player->Inventory->Armor);
                    player->Inventory->Armor = player->Inventory->ContainedItems[i];
                    player->Inventory->ContainedItems.erase(player->Inventory->ContainedItems.begin() + i);

                    SignalActionTextChanged("Armor changed!");
                }
                break;
            case VK_3:
                // Consume health potion
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);

                    int i = 0;
                    for (; i < player->Inventory->ContainedItems.size(); ++i)
                    {
                        if (player->Inventory->ContainedItems[i]->ItemType == TPotion)
                            break;
                    }

                    if (i >= player->Inventory->ContainedItems.size()) break;

                    int restoredHealth = 0;
                    for (const auto statistic : player->Inventory->ContainedItems[i]->Statistics)
                    {
                        if (statistic->Type == HealthRestore)
                        {
                            player->Health += static_cast<int>(statistic->Value);
                            restoredHealth += static_cast<int>(statistic->Value);
                        }
                    }
                    player->Inventory->ContainedItems.erase(player->Inventory->ContainedItems.begin() + i);

                    SignalActionTextChanged("Health potion consumed! Restored " + std::to_string(restoredHealth) + " health.");
                }
                break;
            // If the player accidentally pressed a button or similar...
            default: return;
        }

        // Check if there's any movement...
        if (xMovement != 0 || yMovement != 0)
        {
            const auto playerMoveChanges = MovePlayer(
                Objects[PlayerPosition.Y + yMovement][PlayerPosition.X + xMovement],
                Objects[PlayerPosition.Y][PlayerPosition.X],
                xMovement,
                yMovement
            );
            
            changes.insert(changes.end(), playerMoveChanges.begin(), playerMoveChanges.end());
        }

        const auto enemyMoveChanges = MoveEnemies();
        changes.insert(changes.end(), enemyMoveChanges.begin(), enemyMoveChanges.end());
        
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
                                SignalActionTextChanged("Picked up and equipped a weapon.");
                            }
                            else
                            {
                                player->Inventory->ContainedItems.push_back(item);
                                SignalActionTextChanged("Picked up a weapon.");
                            }
                            break;
                        case TArmor:
                            if (player->Inventory->Armor == nullptr)
                            {
                                player->Inventory->Armor = item;
                                SignalActionTextChanged("Picked up and equipped an armor.");
                            }
                            else
                            {
                                player->Inventory->ContainedItems.push_back(item);
                                SignalActionTextChanged("Picked up an armor.");
                            }
                            break;
                        default:
                            player->Inventory->ContainedItems.push_back(item);
                            SignalActionTextChanged("Picked up an item.");
                            break;
                        }
                    }
                    else
                    {
                        player->Inventory->ContainedItems.push_back(item);
                        SignalActionTextChanged("Picked up an item.");
                    }
            
                    changes.push_back(EnvironmentChange({object->Position.X, object->Position.Y}, {playerObject->Position.X, playerObject->Position.Y}, true, false));
                    object.reset(new Object(object->Position, TNothing));
                    std::swap(object, playerObject);
            
                    moved = true;
                    break;
                }
            case TEnemy:
                // Fight enemy
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(playerObject);
                    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(object);

                    SignalActionTextChanged("Hit an enemy!");
                    bool killedEnemy = Combat::PlayerHitsEnemy(player, enemy);
                    if (killedEnemy)
                    {
                        SignalActionTextChanged("Killed an enemy!");

                        if (!enemy->DroppableItems.empty())
                        {
                            // Take all items
                            player->Inventory->ContainedItems.insert(player->Inventory->ContainedItems.end(), enemy->DroppableItems.begin(), enemy->DroppableItems.end());

                            SignalActionTextChanged("Picked up items the enemy has dropped.");
                        }
                        
                        // Remove the enemy
                        int i = 0;
                        while (i < EnemyPositions.size())
                        {
                            if (EnemyPositions[i].X == object->Position.X &&
                                EnemyPositions[i].Y == object->Position.Y) break;
                            ++i;
                        }

                        if (i < EnemyPositions.size())
                            EnemyPositions.erase(EnemyPositions.begin() + i);
                        
                        changes.push_back(EnvironmentChange({object->Position.X, object->Position.Y}, {playerObject->Position.X, playerObject->Position.Y}, true, false));
                        object.reset(new Object(object->Position, TNothing));
                        std::swap(object, playerObject);

                        moved = true;
                    }
                    else
                    {
                        SignalActionTextChanged("[Enemy HP: " + std::to_string(enemy->Health) + "]");
                    }
                }
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

    std::vector<EnvironmentChange> MoveEnemies()
    {
        std::vector<EnvironmentChange> changes;
        
        for (int i = 0; i < EnemyPositions.size(); ++i)
        {
            const auto enemyChanges = MoveEnemy(EnemyPositions[i]);
            changes.insert(changes.end(), enemyChanges.begin(), enemyChanges.end());
        }
        
        return changes;
    }

    std::vector<EnvironmentChange> MoveEnemy(Coordinates& enemyPosition)
    {
        std::vector<EnvironmentChange> changes;
        
        std::shared_ptr<Object>& enemyObject = Objects[enemyPosition.Y][enemyPosition.X];
        std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(enemyObject);

        const bool isEnemyNextToPlayer = abs(PlayerPosition.Y - enemyPosition.Y + PlayerPosition.X - enemyPosition.X) == 1;
        if (enemy->InCombat && isEnemyNextToPlayer)
        {
            std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);
            bool killedPlayer = Combat::EnemyHitsPlayer(enemy, player);

            if (killedPlayer)
            {
                SignalActionTextChanged("You've died! Game over. Press enter to exit.");
                getchar();
                exit(0);
            }
            
            return changes;
        }
        else
        {
            enemy->InCombat = false;    
        }
        
        int direction = rand() % 4 + 1, xMovement = 0, yMovement = 0;
        switch (direction)
        {
        case 1: // Left
            xMovement = -1;
            break;
        case 2: // Bottom
            yMovement = 1;
            break;
        case 3: // Right
            xMovement = 1;
            break;
        case 4: // Top
            yMovement = -1;
            break;
        }

        std::shared_ptr<Object>& object = Objects[enemyObject->Position.Y + yMovement][enemyObject->Position.X + xMovement];
        
        bool moved = false;
        switch (object->GetType())
        {
            case TEnemy:
            case TWall:
                return changes;
            case TNothing:
            case TDoor:
                changes.push_back(EnvironmentChange({object->Position.X, object->Position.Y}, {enemyObject->Position.X, enemyObject->Position.Y}));
                std::swap(object, enemyObject);
                moved = true;
                break;
            case TItem:
                {
                    std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(object);

                    enemy->DroppableItems.push_back(item);
            
                    changes.push_back(EnvironmentChange({object->Position.X, object->Position.Y}, {enemyObject->Position.X, enemyObject->Position.Y}, true, false));
                    object.reset(new Object(object->Position, TNothing));
                    std::swap(object, enemyObject);
            
                    moved = true;
                    break;
                }
            case TPlayer:
                // Combat
                {
                    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(Objects[PlayerPosition.Y][PlayerPosition.X]);
                    Combat::EnemyHitsPlayer(enemy, player);
                }
            break;
        }

        if (moved)
        {
            object->Position.X += xMovement;
            object->Position.Y += yMovement;
            enemyObject->Position.X -= xMovement;
            enemyObject->Position.Y -= yMovement;

            enemyPosition.X += xMovement;
            enemyPosition.Y += yMovement;
        }

        return changes;
    }
private:
    Coordinates _dimensions;
};
