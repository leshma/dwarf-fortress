#pragma once
#include <memory>

#include "Inventory.h"
#include "Object.h"

/**
 * \brief A class representing the Player of the game.
 */
class Player : public Object
{
public:
    // Players current health points.
    int Health;

    // Players inventory.
    std::unique_ptr<Inventory> Inventory;

    Player(Coordinates position, int health, std::unique_ptr<class Inventory> inventory);
};
