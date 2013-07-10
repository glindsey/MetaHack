#ifndef MAPCORRIDOR_H
#define MAPCORRIDOR_H

#include <SFML/Graphics.hpp>

#include "MapFeature.h"

class MapCorridor : public MapFeature
{
  public:
    MapCorridor(Map& m);
    virtual ~MapCorridor();

    /// Create a corridor of random length, given starting coordinates
    /// and direction.
    /// Checks to make sure the corridor does not intersect existing features.
    /// If successful, puts the coordinates in resultCoords and the endpoint into
    /// endingCoords.
    /// If not successful, returns false.
    virtual bool create(GeoVector vec) override;

    /// Get this corridor's ending coordinates.
    sf::Vector2i const& getEndingCoords() const;

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    static unsigned int maxLength;
    static unsigned int minLength;
    static unsigned int maxRetries;
};

#endif // MAPCORRIDOR_H
