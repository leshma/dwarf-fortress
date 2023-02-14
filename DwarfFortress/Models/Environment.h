#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>
#include <boost/signals2.hpp>

#include "Combat.h"
#include "Coordinates.h"
#include "Door.h"
#include "Enemy.h"
#include "Object.h"
#include "Player.h"
#include "Room.h"
#include "Wall.h"
#include "../Utilities/AdditionalVirtualKeyCodes.h"
#include "../ViewModels/EnvironmentChange.h"

// This class is used to process and gather data about the environment.
class Environment
{
public:
    // A grid containing all of the objects in the environment.
    std::vector<std::vector<std::shared_ptr<Object>>> Objects;
    
    // Current player's position in the environment.
    Coordinates PlayerPosition;

    // Current enemy positions in the environment.
    std::vector<Coordinates> EnemyPositions;

    /**
     * \brief Signal that's fired whenever the environment has changed.
     */
    boost::signals2::signal<void (std::vector<EnvironmentChange>)> SignalEnvironmentChanged;

    /**
     * \brief Signal that's fired whenever the player's status has changed.
     */
    boost::signals2::signal<void ()> SignalStatusChanged;

    /**
     * \brief Signal that's fired whenever the action text has changed.
     */
    boost::signals2::signal<void (std::string)> SignalActionTextChanged;

    Environment(Coordinates dimensions, std::vector<std::shared_ptr<Object>> environmentObjects,
        std::vector<std::shared_ptr<Room>> environmentRooms);

    /**
     * \brief The game's tick. Each movement by the player is processed here.
     */
    void MoveTick(WORD keyPressed);
    std::vector<EnvironmentChange> MovePlayer(std::shared_ptr<Object>& object, std::shared_ptr<Object>& playerObject, int xMovement, int yMovement);
    std::vector<EnvironmentChange> MoveEnemies();
    std::vector<EnvironmentChange> MoveEnemy(Coordinates& enemyPosition);
private:
    Coordinates _dimensions;
};
