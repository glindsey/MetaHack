#ifndef MAPCROSSCORRIDOR_H
#define MAPCROSSCORRIDOR_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapCrossCorridor : public MapFeature
{
  friend class MapFeature;

public:

protected:
  /// Create two intersecting corridor of random length, given starting
  /// coordinates and direction.
  /// Checks to make sure the corridors do not intersect existing features.
  /// If successful, puts the coordinates in resultCoords.
  /// If not successful, throws a MapFeatureException.
  MapCrossCorridor(Map& m, PropertyDictionary const& settings, GeoVector vec);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  static unsigned int maxLength;
  static unsigned int minLength;
  static unsigned int maxRetries;
};

#endif // MAPCROSSCORRIDOR_H
