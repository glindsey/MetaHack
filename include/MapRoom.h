#ifndef MAPROOM_H
#define MAPROOM_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapRoom : public MapFeature
{
  friend class MapFeature;

public:

protected:
  /// Create a rectangular room of random size adjacent to starting
  /// coordinates in the direction indicated.
  /// Checks to make sure the box does not intersect any existing features.
  /// Attempts to create the box max_retries times before giving up.
  MapRoom(Map& m, PropertyDictionary const& settings, GeoVector vec);

private:

  static unsigned int max_width;
  static unsigned int min_width;
  static unsigned int max_height;
  static unsigned int min_height;
  static unsigned int max_retries;
};

#endif // MAPROOM_H
