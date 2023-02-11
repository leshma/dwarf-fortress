#pragma once
#include <vector>

#include "Item.h"
#include "../Statistics/Statistic.h"

class EquippableItem : Item
{
public:
    std::vector<Statistic> Statistics;
};
