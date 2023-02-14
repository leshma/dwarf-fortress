#include "Object.h"

Object::Object(Coordinates position, ObjectType type): Position(position), _objectType(type)
{  }

ObjectType Object::GetType() const
{ return _objectType; }

Object::~Object() = default;
