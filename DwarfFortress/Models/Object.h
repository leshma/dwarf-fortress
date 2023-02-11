#pragma once
#include "Coordinates.h"
#include "ObjectType.h"

class Object
{
public:
    // Contains position in the environment and similar stats (general to all objects)
    Coordinates Position;

    Object() : Position({1, 1}), _objectType(TNothing) {  }

    Object(Coordinates position, ObjectType type) : Position(position), _objectType(type) {  }

    ObjectType GetType() const { return _objectType; }

    virtual ~Object() = default;

private:
    ObjectType _objectType;
};
