#include "Player.h"

Player::Player(Coordinates position, int health, std::unique_ptr<::Inventory> inventory): Object(position, TPlayer), Health(health), Inventory(std::move(inventory))
{
        
}
