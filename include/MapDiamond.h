#ifndef MAPDIAMOND_H
#define MAPDIAMOND_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapDiamond : public MapFeature
{
  friend class MapFeature;

public:

protected:
  /// Create a diamond-shaped room of random size adjacent to starting
  /// coordinates in the direction indicated.
  /// Checks to make sure the box does not intersect any existing features.
  /// Attempts to create the box maxFeatureRetries times before giving up.
  MapDiamond(Map& m, PropertyDictionary const& settings, GeoVector vec);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  static unsigned int maxHalfSize;
  static unsigned int minHalfSize;
  static unsigned int maxRetries;
};

#endif // MAPDIAMOND_H
