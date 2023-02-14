#pragma once
#include <memory>
#include <vector>

#include "Items/Item.h"

/**
 * \brief A class representing the Player inventory.
 */
class Inventory
{
public:
    // Currently equipped Weapon by the Player.
    std::shared_ptr<Item> Weapon;

    // Currently equipped Armor by the Player.
    std::shared_ptr<Item> Armor;

    // Vector containing all of the collected items (weapons, armors and consumables) except for the currently equipped ones.
    std::vector<std::shared_ptr<Item>> ContainedItems;
};
