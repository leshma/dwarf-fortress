#pragma once
#include "Coordinates.h"
#include "Direction.h"

class Room
{
public:
    Coordinates From;
    Coordinates To;
    Direction EntryPoint;

    Room(Coordinates from, Coordinates to, enum Direction entryPoint)
        : From(from), To(to), EntryPoint(entryPoint)
    {  }
};
