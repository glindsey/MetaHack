#ifndef MAPDONUTROOM_H
#define MAPDONUTROOM_H

#include "stdafx.h"

#include "map/MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapDonutRoom : public MapFeature
{
  friend class MapFeature;

public:

protected:
  /// Create a donut-shaped room of random size adjacent to starting
  /// coordinates in the direction indicated.
  /// Checks to make sure the box does not intersect any existing features.
  /// Attempts to create the box max_retries times before giving up.
  MapDonutRoom(Map& m, PropertyDictionary const& settings, GeoVector vec);

private:
};

#endif // MAPDONUTROOM_H
