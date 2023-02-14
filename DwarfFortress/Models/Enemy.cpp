#include "Enemy.h"

Enemy::Enemy(Coordinates position, int health, std::shared_ptr<Item> weapon, std::shared_ptr<Item> armor,
    std::vector<std::shared_ptr<Item>> droppableItems): NonPlayableCharacter(position, health, std::move(weapon), std::move(armor), TEnemy), DroppableItems(std::move(droppableItems)),
                                                        InCombat(false)
{
        
}
