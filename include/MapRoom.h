#ifndef MAPROOM_H
#define MAPROOM_H

#include <SFML/Graphics.hpp>

#include "MapFeature.h"

class MapRoom : public MapFeature
{
  public:
    MapRoom(Map& m);
    virtual ~MapRoom();

    /// Create a rectangular room of random size adjacent to starting
    /// coordinates in the direction indicated.
    /// Checks to make sure the box does not intersect any existing features.
    /// Attempts to create the box maxFeatureRetries times before giving up.
    virtual bool create(GeoVector vec) override;

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    static unsigned int maxWidth;
    static unsigned int maxHeight;
    static unsigned int minWidth;
    static unsigned int minHeight;
    static unsigned int maxRetries;
};

#endif // MAPROOM_H
