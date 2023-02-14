#pragma once
#include "Object.h"
#include "Items/Item.h"

/**
 * \brief A class representing an NPC object. This can be used to create friendly or hostile characters.
 */
class NonPlayableCharacter : public Object
{
public:
    // NPCs current health points.
    int Health;

    // NPCs currently equipped weapon.
    std::shared_ptr<Item> Weapon;

    // NPCs currently equipped armor.
    std::shared_ptr<Item> Armor;

    NonPlayableCharacter(Coordinates position, int health, std::shared_ptr<Item> weapon, std::shared_ptr<Item> armor,
        ObjectType objectType = TNPC);
};
