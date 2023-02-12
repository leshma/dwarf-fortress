#pragma once
#include <memory>

#include "Enemy.h"
#include "Player.h"

class Combat
{
public:
    static bool PlayerHitsEnemy(std::shared_ptr<Player>& player, std::shared_ptr<Enemy>& enemy)
    {
        int enemyArmor = 0, playerDamage = 0;
        if (enemy->Armor != nullptr)
            enemyArmor = enemy->Armor->GetArmorStat();
        
        const auto playerWeapon = player->Inventory->Weapon;
        if (playerWeapon != nullptr)
        {
            playerDamage += playerWeapon->GetDamageStat();
        }

        const auto playerArmor = player->Inventory->Armor;
        if (playerArmor != nullptr)
        {
            playerDamage += playerArmor->GetDamageStat();
        }

        playerDamage -= enemyArmor;
        if (playerDamage < 0) playerDamage = 0;
        
        enemy->Health -= playerDamage;
        enemy->InCombat = true;

        // Killed an enemy
        if (enemy->Health <= 0) return true;

        return false;
    }
    
    static bool EnemyHitsPlayer(std::shared_ptr<Enemy>& enemy, std::shared_ptr<Player>& player)
    {
        int playerArmor = 0, enemyDamage = 0;
        if (player->Inventory->Armor != nullptr)
            playerArmor = player->Inventory->Armor->GetArmorStat();

        const auto enemyWeapon = enemy->Weapon;
        if (enemyWeapon != nullptr)
        {
            enemyDamage += enemyWeapon->GetDamageStat();
        }

        const auto enemyArmor = enemy->Armor;
        if (enemyArmor != nullptr)
        {
            enemyDamage += enemyArmor->GetDamageStat();
        }

        enemyDamage -= playerArmor;
        if (enemyDamage < 0) enemyDamage = 0;
        
        player->Health -= enemyDamage;
        enemy->InCombat = true;

        // Killed the player
        if (player->Health <= 0) return true;

        return false;
    }
};
