#pragma once
#include <memory>

#include "Inventory.h"
#include "Object.h"

class Player : public Object
{
public:
    int Health;
    std::unique_ptr<Inventory> Inventory;

    Player(Coordinates position, int health, std::unique_ptr<class Inventory> inventory)
        : Object(position, TPlayer), Health(health), Inventory(std::move(inventory))
    {
        
    }
};
