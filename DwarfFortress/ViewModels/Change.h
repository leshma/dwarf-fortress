#pragma once
#include <memory>

#include "../Models/Object.h"

class Change
{
public:
    Coordinates From;
    Coordinates To;

    Change(Coordinates from, Coordinates to) : From(from), To(to) {  }
};
