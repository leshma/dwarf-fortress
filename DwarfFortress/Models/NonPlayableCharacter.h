#pragma once
#include "Object.h"
#include "Items/Item.h"

class NonPlayableCharacter : public Object
{
public:
    int Health;
    std::shared_ptr<Item> Weapon;
    std::shared_ptr<Item> Armor;

    NonPlayableCharacter(Coordinates position, int health, std::shared_ptr<Item> weapon, std::shared_ptr<Item> armor,
        ObjectType objectType = TNPC)
        : Object(position, objectType), Health(health), Weapon(std::move(weapon)), Armor(std::move(armor))
    {
        if (objectType != TNPC && objectType != TEnemy)
            throw "An NPC cannot be of a different type other than TNPC or TEnemy!";
    }
};
