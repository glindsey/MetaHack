#ifndef MAPCORRIDOR_H
#define MAPCORRIDOR_H

#include "stdafx.h"

#include "MapFeature.h"

// Forward declarations
class PropertyDictionary;

class MapCorridor : public MapFeature
{
  friend class MapFeature;

public:

  /// Get this corridor's ending coordinates.
  sf::Vector2i const& getEndingCoords() const;

protected:
  /// Create a corridor of random length, given starting coordinates
  /// and direction.
  /// Checks to make sure the corridor does not intersect existing features.
  /// If successful, puts the coordinates in resultCoords and the endpoint into
  /// endingCoords.
  /// If not successful, throws a MapFeatureException.
  MapCorridor(Map& m, PropertyDictionary const& settings, GeoVector vec);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  static unsigned int maxLength;
  static unsigned int minLength;
  static unsigned int maxRetries;
};

#endif // MAPCORRIDOR_H
