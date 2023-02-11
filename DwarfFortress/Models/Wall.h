#pragma once
#include "Object.h"

class Wall : public Object
{
public:
    Wall() : Object() {}
    Wall(Coordinates position) : Object(position, TWall) {}
};
