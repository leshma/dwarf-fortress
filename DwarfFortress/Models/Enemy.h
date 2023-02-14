#pragma once
#include <utility>

#include "NonPlayableCharacter.h"

/**
 * \brief A class representing an Enemy in the game.
 */
class Enemy : public NonPlayableCharacter
{
public:
    // Items that can be dropped when killing an enemy.
    std::vector<std::shared_ptr<Item>> DroppableItems;

    // Indicates whether the Enemy is in combat mode.
    bool InCombat;

    Enemy(Coordinates position, int health, std::shared_ptr<Item> weapon, std::shared_ptr<Item> armor,
        std::vector<std::shared_ptr<Item>> droppableItems);
};
