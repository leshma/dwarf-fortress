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
};
