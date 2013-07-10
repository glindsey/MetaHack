#include "MapGenerator.h"

#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "ErrorHandler.h"
#include "MapCorridor.h"
#include "MapDiamond.h"
#include "MapRoom.h"
#include "MapTile.h"

typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapGenerator::Impl
{
  Impl(Map& m) : gameMap(m) {}

  /// Choose one of two alternatives at random.
  template <class T> T chooseRandom (T a, T b)
  {
    uniform_int_dist choose(0, 1);
    int choice = choose(the_RNG);
    return (choice ? a : b);
  }

  /// Choose one of three alternatives at random.
  template <class T> T chooseRandom (T a, T b, T c)
  {
    uniform_int_dist choose(0, 2);
    int choice = choose(the_RNG);
    switch (choice)
    {
      case 0: return a;
      case 1: return b;
      case 2: return c;

      default: return b;  // should not happen, here to shut compiler up
    }
  }

  /// Fill map with stone.
  void clearMap()
  {
    for (int y = 0; y < gameMap.get_size().y; ++y)
    {
      for (int x = 0; x < gameMap.get_size().x; ++x)
      {
        MapTile& tile = gameMap.get_tile(x, y);
        tile.set_type(MapTileType::WallStone);
      }
    }
  }

  /// Choose a random map feature and find a random place to tack on a new one.
  bool getGrowthVector(GeoVector& growthVector)
  {
    unsigned int numRetries = 0;
    sf::Vector2i const& mapSize = gameMap.get_size();

    while (numRetries < limits.maxAdjacentRetries)
    {
      ++numRetries;

      MapFeature& feature = gameMap.get_random_map_feature();
      if (feature.get_num_growth_vectors() > 0)
      {
        GeoVector vec = feature.get_random_growth_vector();

        // Check the validity of the vector by looking at adjacent tiles.
        // It is valid if the adjacent tile is NOT empty.
        bool vecOkay = false;
        switch (vec.direction)
        {
        case Direction::North:
          if (vec.startPoint.y > 0)
          {
            MapTile& checkTile = gameMap.get_tile(vec.startPoint.x,
                                                 vec.startPoint.y - 1);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        case Direction::East:
          if (vec.startPoint.x < mapSize.x - 1)
          {
            MapTile& checkTile = gameMap.get_tile(vec.startPoint.x + 1,
                                                 vec.startPoint.y);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        case Direction::South:
          if (vec.startPoint.y < mapSize.y - 1)
          {
            MapTile& checkTile = gameMap.get_tile(vec.startPoint.x,
                                                 vec.startPoint.y + 1);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        case Direction::West:
          if (vec.startPoint.x > 0)
          {
            MapTile& checkTile = gameMap.get_tile(vec.startPoint.x - 1,
                                                 vec.startPoint.y);
            vecOkay = !checkTile.is_empty_space();
          }
          break;
        default:
          break;
        }

        if (vecOkay)
        {
          // Use this growth point.
          growthVector.startPoint = vec.startPoint;
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
    sf::Vector2i mapSize = gameMap.get_size();
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
    sf::Vector2i mapSize = gameMap.get_size();
    uniform_int_dist xDist(1, mapSize.x - 2);
    uniform_int_dist yDist(1, mapSize.y - 2);

    sf::Vector2i coords;

    do
    {
      coords.x = xDist(the_RNG);
      coords.y = yDist(the_RNG);
    } while (gameMap.get_tile(coords.x, coords.y).is_empty_space());

    return coords;
  }

  /// Reference to game map.
  Map& gameMap;

  /// Map feature variables.
  FeatureLimits limits;
};

MapGenerator::MapGenerator(Map& m)
  : impl(new Impl(m))
{

}

MapGenerator::~MapGenerator()
{
  //dtor
}

void MapGenerator::generate()
{
  TRACE("Filling map...");
  // Fill the map with stone.
  impl->clearMap();

  // Create the initial room.
  TRACE("Making starting room...");

  MapFeature& startingRoom =
    impl->gameMap.add_map_feature(new MapRoom(impl->gameMap));

  if (!startingRoom.create(GeoVector(impl->getRandomFilledSquare(),
                                     Direction::Self)))
  {
    FATAL_ERROR("Could not make starting room for player!");
  }

  sf::IntRect startBox = startingRoom.get_coords();
  TRACE("Starting room is at (%d, %d) and is size (%d, %d)",
        startBox.left, startBox.top, startBox.width, startBox.height);

  sf::Vector2i startCoords(startBox.left + (startBox.width / 2),
                           startBox.top + (startBox.height / 2));

  impl->gameMap.set_start_location(startCoords);

  // Continue with additional map features.
  TRACE("Making additional map features...");

  unsigned int mapFeatures = 0;

  while (mapFeatures < impl->limits.maxFeatures)
  {
    GeoVector nextGrowthVector;
    MapFeature* feature = nullptr;

    if (impl->getGrowthVector(nextGrowthVector))
    {
      uniform_int_dist chooseAFeature(0, 4);
      int chosenFeature = chooseAFeature(the_RNG);

      switch (chosenFeature)
      {
      case 3:
        {
          feature = new MapDiamond(impl->gameMap);
        }
        break;

      case 4:
        {
          feature = new MapCorridor(impl->gameMap);
        }
        break;

      default:
        {
          feature = new MapRoom(impl->gameMap);
        }
        break;
      }
    }

    if (feature != nullptr)
    {
      if (feature->create(nextGrowthVector))
      {
        impl->gameMap.add_map_feature(feature);
      }
      else
      {
        delete feature;
      }
    }
    ++mapFeatures;
  }
}
