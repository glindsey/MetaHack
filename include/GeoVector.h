#ifndef GEOVECTOR_H
#define GEOVECTOR_H

#include <SFML/Graphics.hpp>

#include "Direction.h"

/// A GeoVector is, simply put, a geometric vector on a 2-d plane.
/// It has a starting point and a direction.  That's about it.
struct GeoVector
{
  GeoVector()
  {
    startPoint = sf::Vector2i(0, 0);
    direction = Direction::Self;
  }

  GeoVector(sf::Vector2i start, Direction dir)
  {
    startPoint = start;
    direction = dir;
  }

  GeoVector(int x, int y, Direction dir)
  {
    startPoint = sf::Vector2i(x, y);
    direction = dir;
  }

  bool operator==(const GeoVector &other) const
  {
    return (startPoint == other.startPoint) &&
           (direction == other.direction);
  }

  bool operator!=(const GeoVector &other) const
  {
    return !(*this == other);
  }

  sf::Vector2i startPoint;
  Direction direction;
};

#endif // GEOVECTOR_H
