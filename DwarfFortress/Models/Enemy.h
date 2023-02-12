#pragma once
#include <utility>

#include "NonPlayableCharacter.h"

class Enemy : public NonPlayableCharacter
{
public:
    std::vector<std::shared_ptr<Item>> DroppableItems;
    bool InCombat;

    Enemy(Coordinates position, int health, std::shared_ptr<Item> weapon, std::shared_ptr<Item> armor,
        std::vector<std::shared_ptr<Item>> droppableItems)
        : NonPlayableCharacter(position, health, std::move(weapon), std::move(armor), TEnemy), DroppableItems(std::move(droppableItems)),
          InCombat(false)
    {
        
    }
};
