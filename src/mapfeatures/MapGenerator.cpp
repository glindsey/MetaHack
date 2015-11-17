#include "MapGenerator.h"

#include <boost/log/trivial.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "MapCorridor.h"
#include "MapDiamond.h"
#include "MapDonutRoom.h"
#include "MapLRoom.h"
#include "MapRoom.h"

#include "App.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "PropertyDictionary.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapGenerator::Impl
{
  Impl(Map& m) : game_map(m) {}

  /// Fill map with stone.
  void clearMap()
  {
    for (int y = 0; y < game_map.get_size().y; ++y)
    {
      for (int x = 0; x < game_map.get_size().x; ++x)
      {
        auto& tile = game_map.get_tile(x, y);
        tile.set_type("MTWallStone");
      }
    }
  }

  /// Choose a random map feature and find a random place to tack on a new one.
  bool getGrowthVector(GeoVector& growthVector)
  {
    unsigned int numRetries = 0;
    sf::Vector2i const& mapSize = game_map.get_size();

    while (numRetries < limits.maxAdjacentRetries)
    {
      ++numRetries;

      MapFeature& feature = game_map.get_random_map_feature();
      if (feature.get_num_growth_vectors() > 0)
      {
        GeoVector vec = feature.get_random_growth_vector();

        // Check the validity of the vector by looking at adjacent tiles.
        // It is valid if the adjacent tile is NOT empty.
        bool vecOkay = false;
        switch (vec.direction)
        {
        case Direction::North:
          if (vec.start_point.y > 0)
          {
            auto& checkTile = game_map.get_tile(vec.start_point.x,
                                                vec.start_point.y - 1);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        case Direction::East:
          if (vec.start_point.x < mapSize.x - 1)
          {
            auto& checkTile = game_map.get_tile(vec.start_point.x + 1,
                                                vec.start_point.y);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        case Direction::South:
          if (vec.start_point.y < mapSize.y - 1)
          {
            auto& checkTile = game_map.get_tile(vec.start_point.x,
                                                vec.start_point.y + 1);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        case Direction::West:
          if (vec.start_point.x > 0)
          {
            auto& checkTile = game_map.get_tile(vec.start_point.x - 1,
                                                vec.start_point.y);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        default:
          break;
        }

        if (vecOkay)
        {
          // Use this growth point.
          growthVector.start_point = vec.start_point;
          growthVector.direction = vec.direction;
          return true;
        }
        else
        {
          // This is no longer a viable growth point, so erase it.
          feature.erase_growth_vector(vec);
        }
      }
    }

    return false;
  }

  /// Get a random square on the map, regardless of usage, excluding the map
  /// boundaries.
  sf::Vector2i getRandomSquare()
  {
    sf::Vector2i mapSize = game_map.get_size();
    uniform_int_dist xDist(1, mapSize.x - 2);
    uniform_int_dist yDist(1, mapSize.y - 2);

    sf::Vector2i coords;
    coords.x = xDist(the_RNG);
    coords.y = yDist(the_RNG);
    return coords;
  }

  /// Get a random filled square on the map, excluding the map boundaries.
  /// @warning Assumes there's at least one non-empty space on the map,
  ///          or function will loop indefinitely!
  sf::Vector2i getRandomFilledSquare()
  {
    sf::Vector2i mapSize = game_map.get_size();
    uniform_int_dist xDist(1, mapSize.x - 2);
    uniform_int_dist yDist(1, mapSize.y - 2);

    sf::Vector2i coords;

    do
    {
      coords.x = xDist(the_RNG);
      coords.y = yDist(the_RNG);
    } while (game_map.get_tile(coords.x, coords.y).is_empty_space());

    return coords;
  }

  /// Reference to game map.
  Map& game_map;

  /// Map feature variables.
  FeatureLimits limits;
};

MapGenerator::MapGenerator(Map& m)
  : pImpl(NEW Impl(m))
{

}

MapGenerator::~MapGenerator()
{
  //dtor
}

void MapGenerator::generate()
{
  PropertyDictionary feature_settings;

  TRACE("Filling map...");
  // Fill the map with stone.
  pImpl->clearMap();

  // Create the initial room.
  TRACE("Making starting room...");

  MapFeature& startingRoom =
    pImpl->game_map.add_map_feature(NEW MapRoom(pImpl->game_map, feature_settings));

  if (!startingRoom.create(GeoVector(pImpl->getRandomFilledSquare(),
                                     Direction::Self)))
  {
    throw std::exception("Could not make starting room for player!");
  }

  sf::IntRect startBox = startingRoom.get_coords();
  TRACE("Starting room is at (%d, %d) and is size (%d, %d)",
        startBox.left, startBox.top, startBox.width, startBox.height);

  sf::Vector2i startCoords(startBox.left + (startBox.width / 2),
                           startBox.top + (startBox.height / 2));

  pImpl->game_map.set_start_coords(startCoords);

  // Continue with additional map features.
  TRACE("Making additional map features...");

  unsigned int mapFeatures = 0;

  while (mapFeatures < pImpl->limits.maxFeatures)
  {
    GeoVector nextGrowthVector;
    MapFeature* feature = nullptr;

    uniform_int_dist chooseAFeature(0, 8);
    int chosen_feature = chooseAFeature(the_RNG);
    switch (chosen_feature)
    {
    case 0:
      feature_settings.set<std::string>("type", "room_diamond");
      break;

    case 1:
      feature_settings.set<std::string>("type", "room_l");
      break;

    case 2:
      feature_settings.set<std::string>("type", "room_torus");
      break;

    case 3:
    case 4:
    case 5:
      feature_settings.set<std::string>("type", "corridor");
      break;

    default:
      feature_settings.set<std::string>("type", "room");
      break;
    }

    add_feature(chosen_feature, feature_settings);
    ++mapFeatures;
  }
}

bool MapGenerator::add_feature(int chosen_feature, PropertyDictionary const& feature_settings)
{
  GeoVector nextGrowthVector;
  std::unique_ptr<MapFeature> feature;

  if (pImpl->getGrowthVector(nextGrowthVector))
  {
    std::string feature_type = feature_settings.get<std::string>("type");

    if (feature_type == "room")
    {
      feature.reset(NEW MapRoom(pImpl->game_map, feature_settings));
    }
    else if (feature_type == "corridor")
    {
      feature.reset(NEW MapCorridor(pImpl->game_map, feature_settings));
    }
    else if (feature_type == "room_l")
    {
      feature.reset(NEW MapLRoom(pImpl->game_map, feature_settings));
    }
    else if (feature_type == "room_diamond")
    {
      feature.reset(NEW MapDiamond(pImpl->game_map, feature_settings));
    }
    else if (feature_type == "room_torus")
    {
      feature.reset(NEW MapDonutRoom(pImpl->game_map, feature_settings));
    }
    else
    {
      MINOR_ERROR("Unknown feature type \"%s\" requested", feature_type.c_str());
    }
  }

  if (feature)
  {
    if (feature->create(nextGrowthVector))
    {
      pImpl->game_map.add_map_feature(feature.get());
      feature.release();
      return true;
    }
  }

  return false;
}