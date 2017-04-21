#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "stdafx.h"

#include "lua/LuaObject.h"
#include "map/Map.h"

// Forward declarations
struct GeoVector;

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
  /// Fill map with stone.
  void clearMap();

  /// Choose a random map feature and find a random place to tack on a new one.
  bool getGrowthVector(GeoVector& growthVector);

  /// Get a random square on the map, regardless of usage, excluding the map
  /// boundaries.
  IntVec2 getRandomSquare();

  /// Get a random filled square on the map, excluding the map boundaries.
  /// @warning Assumes there's at least one non-empty space on the map,
  ///          or function will loop indefinitely!
  IntVec2 getRandomFilledSquare();


private:
  /// Reference to game map.
  Map& m_game_map;

  /// Map feature variables.
  FeatureLimits m_limits;
};

#endif // MAPGENERATOR_H
