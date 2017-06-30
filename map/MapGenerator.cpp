#include "stdafx.h"

#include "map/MapGenerator.h"

#include "game/App.h"
#include "map/MapFeature.h"
#include "maptile/MapTile.h"
#include "properties/PropertyDictionary.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"

MapGenerator::MapGenerator(Map& m)
  : m_game_map{ m }
{}

MapGenerator::~MapGenerator()
{
  //dtor
}

void MapGenerator::generate()
{
  PropertyDictionary feature_settings;

  CLOG(TRACE, "MapGenerator") << "Filling map...";
  // Fill the map with stone.
  clearMap();

  // Create the initial room.
  CLOG(TRACE, "MapGenerator") << "Making starting room...";

  GeoVector startingVector{ getRandomFilledSquare(), Direction::Self };
  feature_settings.set("type", "room");
  feature_settings.set("min_width", 5);
  feature_settings.set("max_width", 5);
  feature_settings.set("min_height", 5);
  feature_settings.set("max_height", 5);
  feature_settings.set("max_retries", 100);
  std::unique_ptr<MapFeature> startRoom = MapFeature::construct(m_game_map, feature_settings, startingVector);

  if (!startRoom)
  {
    CLOG(FATAL, "MapGenerator") << "Could not make starting room for player!";
  }

  MapFeature& startingRoom = m_game_map.addMapFeature(startRoom.get());
  startRoom.release();

  sf::IntRect startBox = startingRoom.getCoords();
  CLOG(TRACE, "MapGenerator") << "Starting room is at " << startBox;

  IntVec2 startCoords(startBox.left + (startBox.width / 2),
                           startBox.top + (startBox.height / 2));

  CLOG(TRACE, "MapGenerator") << "Setting start coords to " << startCoords;

  m_game_map.setStartCoords(startCoords);

  // Continue with additional map features.
  CLOG(TRACE, "MapGenerator") << "Making additional map features...";

  unsigned int mapFeatures = 0;

  while (mapFeatures < m_limits.maxFeatures)
  {
    GeoVector nextGrowthVector;

    UniformIntDist chooseAFeature(0, 8);
    feature_settings.clear();
    int chosen_feature = chooseAFeature(the_RNG);
    switch (chosen_feature)
    {
      case 0:
        feature_settings.set("type", "room_diamond");
        feature_settings.set("max_half_size", 4);
        feature_settings.set("min_half_size", 2);
        feature_settings.set("max_retries", 100);
        break;

      case 1:
        feature_settings.set("type", "room_l");
        feature_settings.set("horiz_leg_max_width", 20);
        feature_settings.set("horiz_leg_min_width", 10);
        feature_settings.set("horiz_leg_max_height", 7);
        feature_settings.set("horiz_leg_min_height", 3);
        feature_settings.set("vert_leg_max_width", 7);
        feature_settings.set("vert_leg_min_width", 3);
        feature_settings.set("vert_leg_max_height", 20);
        feature_settings.set("vert_leg_min_height", 10);
        feature_settings.set("max_retries", 500);
        break;

      case 2:
        feature_settings.set("type", "room_torus");
        feature_settings.set("max_width", 20);
        feature_settings.set("min_width", 7);
        feature_settings.set("max_height", 20);
        feature_settings.set("min_height", 7);
        feature_settings.set("min_hole_size", 5);
        feature_settings.set("max_retries", 500);
        break;

      case 3:
      case 4:
      case 5:
        feature_settings.set("type", "corridor");
        feature_settings.set("max_length", 48);
        feature_settings.set("min_length", 3);
        feature_settings.set("max_retries", 100);
        break;

      default:
        feature_settings.set("type", "room");
        feature_settings.set("max_width", 15);
        feature_settings.set("min_width", 3);
        feature_settings.set("max_height", 15);
        feature_settings.set("min_height", 3);
        feature_settings.set("max_retries", 100);
        break;
    }

    add_feature(feature_settings);
    ++mapFeatures;
  }
}

bool MapGenerator::add_feature(PropertyDictionary const& feature_settings)
{
  GeoVector nextGrowthVector;
  std::unique_ptr<MapFeature> feature;

  if (getGrowthVector(nextGrowthVector))
  {
    feature = MapFeature::construct(m_game_map, feature_settings, nextGrowthVector);
  }

  if (feature)
  {
    m_game_map.addMapFeature(feature.get());
    feature.release();
    return true;
  }

  return false;
}

/// Fill map with stone.
void MapGenerator::clearMap()
{
  for (int y = 0; y < m_game_map.getSize().y; ++y)
  {
    for (int x = 0; x < m_game_map.getSize().x; ++x)
    {
      auto& tile = m_game_map.getTile({ x, y });
      tile.setTileType("MTWallStone");
    }
  }
}

/// Choose a random map feature and find a random place to tack on a new one.
bool MapGenerator::getGrowthVector(GeoVector& growthVector)
{
  unsigned int numRetries = 0;
  IntVec2 const& mapSize = m_game_map.getSize();

  while (numRetries < m_limits.maxAdjacentRetries)
  {
    ++numRetries;

    MapFeature& feature = m_game_map.getRandomMapFeature();
    if (feature.get_num_growth_vectors() > 0)
    {
      GeoVector vec = feature.get_random_growth_vector();

      // Check the validity of the vector by looking at adjacent tiles.
      // It is valid if the adjacent tile is NOT empty.
      bool vecOkay = false;

      if (vec.direction == Direction::North)
      {
        if (vec.start_point.y > 0)
        {
          auto& checkTile = m_game_map.getTile({ vec.start_point.x, vec.start_point.y - 1 });
          vecOkay = !checkTile.isPassable();
        }
      }
      else if (vec.direction == Direction::East)
      {
        if (vec.start_point.x < mapSize.x - 1)
        {
          auto& checkTile = m_game_map.getTile({ vec.start_point.x + 1, vec.start_point.y });
          vecOkay = !checkTile.isPassable();
        }
      }
      else if (vec.direction == Direction::South)
      {
        if (vec.start_point.y < mapSize.y - 1)
        {
          auto& checkTile = m_game_map.getTile({ vec.start_point.x, vec.start_point.y + 1 });
          vecOkay = !checkTile.isPassable();
        }
      }
      else if (vec.direction == Direction::West)
      {
        if (vec.start_point.x > 0)
        {
          auto& checkTile = m_game_map.getTile({ vec.start_point.x - 1, vec.start_point.y });
          vecOkay = !checkTile.isPassable();
        }
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
IntVec2 MapGenerator::getRandomSquare()
{
  IntVec2 mapSize = m_game_map.getSize();
  UniformIntDist xDist(1, mapSize.x - 2);
  UniformIntDist yDist(1, mapSize.y - 2);

  IntVec2 coords;
  coords.x = xDist(the_RNG);
  coords.y = yDist(the_RNG);
  return coords;
}

/// Get a random filled square on the map, excluding the map boundaries.
/// @warning Assumes there's at least one non-empty space on the map,
///          or function will loop indefinitely!
IntVec2 MapGenerator::getRandomFilledSquare()
{
  IntVec2 mapSize = m_game_map.getSize();
  UniformIntDist xDist(1, mapSize.x - 2);
  UniformIntDist yDist(1, mapSize.y - 2);

  IntVec2 coords;

  do
  {
    coords.x = xDist(the_RNG);
    coords.y = yDist(the_RNG);
  } while (m_game_map.getTile(coords).isPassable());

  return coords;
}
