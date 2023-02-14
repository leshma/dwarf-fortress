#pragma once
#include "StatisticType.h"

// This is used to represent a single statistic for an item.
class Statistic
{
public:
    // The statistic's type. (Damage, Armor, etc.)
    StatisticType Type;
    
    // Value of the statistic.
    double Value;

    Statistic(StatisticType type, double value);
};
