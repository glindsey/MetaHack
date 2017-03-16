#ifndef GEOVECTOR_H
#define GEOVECTOR_H

#include "stdafx.h"

#include "Direction.h"

/// A GeoVector is, simply put, a geometric vector on a 2-d plane.
/// It has a starting point and a direction.  That's about it.
struct GeoVector
{
  GeoVector()
  {
    start_point = IntegerVec2(0, 0);
    direction = Direction::Self;
  }

  GeoVector(IntegerVec2 start, Direction dir)
  {
    start_point = start;
    direction = dir;
  }

  GeoVector(int x, int y, Direction dir)
  {
    start_point = IntegerVec2(x, y);
    direction = dir;
  }

  bool operator==(const GeoVector &other) const
  {
    return (start_point == other.start_point) &&
      (direction == other.direction);
  }

  bool operator!=(const GeoVector &other) const
  {
    return !(*this == other);
  }

  IntegerVec2 start_point;
  Direction direction;
};

#endif // GEOVECTOR_H
