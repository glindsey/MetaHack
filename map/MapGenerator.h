#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "stdafx.h"

#include "lua/LuaObject.h"
#include "map/Map.h"

/// The MapGenerator fills a map with dungeon features.
class MapGenerator
{
public:
  explicit MapGenerator(Map& m);
  virtual ~MapGenerator();

  void generate();

  bool add_feature(PropertyDictionary const& settings);

  struct FeatureLimits
  {
    /// Maximum number of features present on the map.
    unsigned int maxFeatures = 500;

    /// Total times to retry getting an adjacent feature before giving up.
    unsigned int maxAdjacentRetries = 10000;

    /// Total times to retry making a feature before giving up.
    unsigned int maxFeatureRetries = 50;

    // *** Standard rectangular rooms ***************************************
    unsigned int maxBoxWidth = 16;
    unsigned int maxBoxHeight = 16;
    unsigned int minBoxWidth = 3;
    unsigned int minBoxHeight = 3;

    // *** Narrow corridors *************************************************
    unsigned int minCorridorLen = 3;
    unsigned int maxCorridorLen = 50;
  };

protected:
private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // MAPGENERATOR_H
