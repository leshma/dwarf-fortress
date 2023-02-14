#pragma once
#include "Object.h"

/**
 * \brief A class representing a Wall in the game.
 * <br/> Walls cannot be accessed by any character.
 */
class Wall : public Object
{
public:
    Wall();
    Wall(Coordinates position);
};
