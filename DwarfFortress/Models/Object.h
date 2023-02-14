#pragma once
#include "Coordinates.h"
#include "ObjectType.h"

/**
 * \brief A generic class representing all of the objects in the game.
 * <br/> It contains the most important information about an object such as their Position and Object Type.
 */
class Object
{
public:
    // Contains position in the environment and similar stats (general to all objects)
    Coordinates Position;

    Object() : Position({1, 1}), _objectType(TNothing) {  }

    Object(Coordinates position, ObjectType type);

    ObjectType GetType() const;

    virtual ~Object();
private:
    ObjectType _objectType;
};
