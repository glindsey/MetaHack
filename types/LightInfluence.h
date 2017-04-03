#ifndef LIGHTINFLUENCE_H
#define LIGHTINFLUENCE_H

#include "types/Color.h"
#include "types/Vec2.h"

struct LightInfluence
{
  IntVec2 coords;  ///< Coordinates of the light.
  Color color;     ///< Color of the light.
  int intensity;   ///< Intensity of the light.
};

#endif // LIGHTINFLUENCE_H
