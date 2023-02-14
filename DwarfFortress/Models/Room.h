#pragma once
#include "Coordinates.h"
#include "Direction.h"

/**
 * \brief A class representing the Room in the game.
 */
class Room
{
public:
    // From where the Room begins.
    Coordinates From;

    // To where the Room ends.
    Coordinates To;

    // An entry point of the Room.
    Direction EntryPoint;

    Room(Coordinates from, Coordinates to, enum Direction entryPoint);
};
