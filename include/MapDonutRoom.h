#ifndef MAPDONUTROOM_H
#define MAPDONUTROOM_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapDonutRoom : public MapFeature
{
public:
  MapDonutRoom(Map& m, PropertyDictionary const& settings);
  virtual ~MapDonutRoom();

  /// Create a donut-shaped room of random size adjacent to starting
  /// coordinates in the direction indicated.
  /// Checks to make sure the box does not intersect any existing features.
  /// Attempts to create the box max_retries times before giving up.
  virtual bool create(GeoVector vec) override;

protected:
private:
  static unsigned int max_width;
  static unsigned int min_width;
  static unsigned int max_height;
  static unsigned int min_height;
  static unsigned int min_hole_size;
  static unsigned int max_retries;
};

#endif // MAPDONUTROOM_H
