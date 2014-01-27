#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include <memory>

#include "Map.h"

/// The MapGenerator fills a map with dungeon features.
class MapGenerator
{
  public:
    MapGenerator(Map& m);
    virtual ~MapGenerator();

    void generate();

    struct FeatureLimits
    {
      /// Maximum number of features present on the map.
      unsigned int maxFeatures = 1000;

      /// Total times to retry getting an adjacent feature before giving up.
      unsigned int maxAdjacentRetries = 20000;

      /// Total times to retry making a feature before giving up.
      unsigned int maxFeatureRetries = 100;

      // *** Standard rectangular rooms ***************************************
      unsigned int maxBoxWidth = 20;
      unsigned int maxBoxHeight = 20;
      unsigned int minBoxWidth = 3;
      unsigned int minBoxHeight = 3;

      // *** Narrow corridors *************************************************
      unsigned int minCorridorLen = 3;
      unsigned int maxCorridorLen = 60;
    };

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // MAPGENERATOR_H
