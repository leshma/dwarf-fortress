#pragma once

#include "Object.h"

/**
 * \brief This class represents a Door object.
 * <br/> Currently it serves no purpose other than to let the Player through.
 */
class Door : public Object
{
public:
    Door(Coordinates position);
};
