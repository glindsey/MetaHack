#ifndef MAPLROOM_H
#define MAPLROOM_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapLRoom : public MapFeature
{
  friend class MapFeature;

public:

protected:
  /// Create an L-shaped room of random size adjacent to starting
  /// coordinates in the direction indicated.
  /// Checks to make sure the box does not intersect any existing features.
  /// Attempts to create the box maxFeatureRetries times before giving up.
  MapLRoom(Map& m, PropertyDictionary const& settings, GeoVector vec);

private:
};

#endif // MAPLROOM_H
