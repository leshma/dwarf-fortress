#include "EnvironmentChange.h"

EnvironmentChange::EnvironmentChange(Coordinates from, Coordinates to, bool fromObjectDisappears,
    bool toObjectDisappears): From(from), To(to), FromObjectDisappears(fromObjectDisappears), ToObjectDisappears(toObjectDisappears)
{  }
