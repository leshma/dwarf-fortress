#pragma once
#include <memory>
#include <utility>
#include <vector>

#include "ItemType.h"
#include "PersistanceType.h"
#include "../Object.h"
#include "../Statistics/Statistic.h"

/**
 * \brief A class representing an Item in the game.
 * <br/> An Item can be various: Weapons, Armor, Consumables, etc.
 */
class Item : public Object
{
public:
    // A vector containing the item's stats.
    std::vector<std::shared_ptr<Statistic>> Statistics;

    // The type of persistance of this item. (Equippable or Consumable)
    PersistanceType PersistanceType;

    // The type of the Item.
    ItemType ItemType;

    Item(Coordinates position, std::vector<std::shared_ptr<Statistic>> statistics, enum PersistanceType persistanceType, enum ItemType itemType);

    // Returns the full Damage statistic of the Item.
    int GetDamageStat();

    // Returns the full Armor statistic of the Item.
    int GetArmorStat();

    // Returns the full Health statistic of the Item.
    int GetHealthStat();
};
