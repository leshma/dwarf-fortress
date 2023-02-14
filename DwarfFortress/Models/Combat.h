#pragma once
#include <memory>

#include "Enemy.h"
#include "Player.h"

class Combat
{
public:
    static bool PlayerHitsEnemy(std::shared_ptr<Player>& player, std::shared_ptr<Enemy>& enemy);
    static bool EnemyHitsPlayer(std::shared_ptr<Enemy>& enemy, std::shared_ptr<Player>& player);
};
