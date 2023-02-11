#pragma once
#include <memory>
#include <vector>

#include "Items/Item.h"

class Inventory
{
public:
    std::vector<std::shared_ptr<Item>> EquippedItems;
    std::vector<std::shared_ptr<Item>> ContainedItems;
};
