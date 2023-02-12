#pragma once
#include <memory>

#include "../Models/Object.h"

class EnvironmentChange
{
public:
    Coordinates From;
    Coordinates To;
    bool FromObjectDisappears;
    bool ToObjectDisappears;

    EnvironmentChange(Coordinates from, Coordinates to, bool fromObjectDisappears = false, bool toObjectDisappears = false)
        : From(from), To(to), FromObjectDisappears(fromObjectDisappears), ToObjectDisappears(toObjectDisappears) {  }
};
