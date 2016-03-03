#ifndef MAPLROOM_H
#define MAPLROOM_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapLRoom : public MapFeature
{
public:
  MapLRoom(Map& m, PropertyDictionary const& settings);
  virtual ~MapLRoom();

  /// Create an L-shaped room of random size adjacent to starting
  /// coordinates in the direction indicated.
  /// Checks to make sure the box does not intersect any existing features.
  /// Attempts to create the box maxFeatureRetries times before giving up.
  virtual bool create(GeoVector vec) override;

protected:
private:
  static unsigned int horiz_leg_max_width;
  static unsigned int horiz_leg_min_width;
  static unsigned int horiz_leg_max_height;
  static unsigned int horiz_leg_min_height;
  static unsigned int vert_leg_max_width;
  static unsigned int vert_leg_min_width;
  static unsigned int vert_leg_max_height;
  static unsigned int vert_leg_min_height;
  static unsigned int max_retries;
};

#endif // MAPLROOM_H
