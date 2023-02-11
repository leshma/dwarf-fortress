#pragma once

#include "Object.h"

class Door : public Object
{
public:
    Door(Coordinates position) : Object(position, TDoor) {  }
};
