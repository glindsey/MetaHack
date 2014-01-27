#ifndef MAPDIAMOND_H
#define MAPDIAMOND_H

#include <SFML/Graphics.hpp>

#include "MapFeature.h"


class MapDiamond : public MapFeature
{
  public:
    MapDiamond(Map& m);
    virtual ~MapDiamond();
  protected:
    /// Create a diamond-shaped room of random size adjacent to starting
    /// coordinates in the direction indicated.
    /// Checks to make sure the box does not intersect any existing features.
    /// Attempts to create the box maxFeatureRetries times before giving up.
    virtual bool create(GeoVector vec) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    static unsigned int maxHalfSize;
    static unsigned int minHalfSize;
    static unsigned int maxRetries;
};

#endif // MAPDIAMOND_H
