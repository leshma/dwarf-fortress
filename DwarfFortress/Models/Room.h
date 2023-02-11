#pragma once
#include "Coordinates.h"
#include "EntryPoint.h"

class Room
{
public:
    Coordinates From;
    Coordinates To;
    EntryPoint EntryPoint;

    Room(Coordinates from, Coordinates to, enum EntryPoint entryPoint)
        : From(from), To(to), EntryPoint(entryPoint)
    {  }
};
