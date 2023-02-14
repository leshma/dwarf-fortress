#pragma once
#include <memory>

#include "../Models/Object.h"

/**
 * \brief A class that represents a change in the Environment.
 * <br/> It's used by the Environment and the View to quickly assess the required changes to the gameplay.
 */
class EnvironmentChange
{
public:
    // From where the object is moved?
    Coordinates From;
    
    // Towards where the object is moved?
    Coordinates To;

    // Represents whether the From object disappears.
    bool FromObjectDisappears;
    
    // Represents whether the To object disappears.
    bool ToObjectDisappears;

    EnvironmentChange(Coordinates from, Coordinates to, bool fromObjectDisappears = false, bool toObjectDisappears = false);
};
