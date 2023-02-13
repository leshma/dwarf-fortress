#pragma once
#include <memory>
#include <utility>
#include <vector>

#include "ItemType.h"
#include "PersistanceType.h"
#include "../Object.h"
#include "../Statistics/Statistic.h"

class Item : public Object
{
public:
    std::vector<std::shared_ptr<Statistic>> Statistics;
    PersistanceType PersistanceType;
    ItemType ItemType;

    Item(Coordinates position, std::vector<std::shared_ptr<Statistic>> statistics, enum PersistanceType persistanceType, enum ItemType itemType)
        : Object(position, TItem), Statistics(std::move(statistics)), PersistanceType(persistanceType), ItemType(itemType) {  }

    int GetDamageStat()
    {
        int damage = 0;
        for (auto statistic : Statistics)
        {
            if (statistic->Type == Damage)
                damage += static_cast<int>(statistic->Value);
        }
        return damage;
    }

    int GetArmorStat()
    {
        int armor = 0;
        for (auto statistic : Statistics)
        {
            if (statistic->Type == Armor)
                armor += static_cast<int>(statistic->Value);
        }
        return armor;
    }

    int GetHealthStat()
    {
        int health = 0;
        for (auto statistic : Statistics)
        {
            if (statistic->Type == Health)
                health += static_cast<int>(statistic->Value);
        }
        return health;
    }
};
