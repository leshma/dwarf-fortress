#include "Item.h"

Item::Item(Coordinates position, std::vector<std::shared_ptr<Statistic>> statistics, ::PersistanceType persistanceType,
    ::ItemType itemType): Object(position, TItem), Statistics(std::move(statistics)), PersistanceType(persistanceType), ItemType(itemType)
{  }

int Item::GetDamageStat()
{
    int damage = 0;
    for (auto statistic : Statistics)
    {
        if (statistic->Type == Damage)
            damage += static_cast<int>(statistic->Value);
    }
    return damage;
}

int Item::GetArmorStat()
{
    int armor = 0;
    for (auto statistic : Statistics)
    {
        if (statistic->Type == Armor)
            armor += static_cast<int>(statistic->Value);
    }
    return armor;
}

int Item::GetHealthStat()
{
    int health = 0;
    for (auto statistic : Statistics)
    {
        if (statistic->Type == Health)
            health += static_cast<int>(statistic->Value);
    }
    return health;
}
