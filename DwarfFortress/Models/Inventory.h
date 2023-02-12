#pragma once
#include <memory>
#include <vector>

#include "Items/Item.h"

class Inventory
{
public:
    std::shared_ptr<Item> Weapon;
    std::shared_ptr<Item> Armor;
    std::vector<std::shared_ptr<Item>> ContainedItems;
};
