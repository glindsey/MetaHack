#include "MapRoom.h"

#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "ErrorHandler.h"
#include "MapTile.h"
#include "ThingFactory.h"

// Include needed for testing code
#include "Sconce.h"

// Static declarations
unsigned int MapRoom::maxWidth = 20;
unsigned int MapRoom::maxHeight = 20;
unsigned int MapRoom::minWidth = 3;
unsigned int MapRoom::minHeight = 3;
unsigned int MapRoom::maxRetries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapRoom::Impl
{
  bool dummy;
};

MapRoom::MapRoom(Map& m)
  : MapFeature(m), impl(new Impl())
{
  //ctor
}

MapRoom::~MapRoom()
{
  //dtor
}

bool MapRoom::create(GeoVector vec)
{
  unsigned int numTries = 0;
  uniform_int_dist wDist(minWidth, maxWidth);
  uniform_int_dist hDist(minHeight, maxHeight);

  sf::Vector2i& startingCoords = vec.startPoint;
  Direction& direction = vec.direction;

  while (numTries < maxRetries)
  {
    sf::Vector2i box_size(wDist(the_RNG), hDist(the_RNG));
    uniform_int_dist xOffsetDist(0, box_size.x - 1);
    uniform_int_dist yOffsetDist(0, box_size.y - 1);

    int xMin, xMax, yMin, yMax;
    int xOffset = xOffsetDist(the_RNG);
    int yOffset = yOffsetDist(the_RNG);

    switch (direction)
    {
    case Direction::North:
      yMax = startingCoords.y - 1;
      yMin = yMax - (box_size.y - 1);
      xMin = startingCoords.x - xOffset;
      xMax = xMin + (box_size.x - 1);
      break;
    case Direction::South:
      yMin = startingCoords.y + 1;
      yMax = yMin + (box_size.y - 1);
      xMin = startingCoords.x - xOffset;
      xMax = xMin + (box_size.x - 1);
      break;
    case Direction::West:
      xMax = startingCoords.x - 1;
      xMin = xMax - (box_size.x - 1);
      yMin = startingCoords.y - yOffset;
      yMax = yMin + (box_size.y - 1);
      break;
    case Direction::East:
      xMin = startingCoords.x + 1;
      xMax = xMin + (box_size.x - 1);
      yMin = startingCoords.y - yOffset;
      yMax = yMin + (box_size.y - 1);
      break;
    case Direction::Self:
      xMin = startingCoords.x - xOffset;
      xMax = xMin + (box_size.x - 1);
      yMin = startingCoords.y - yOffset;
      yMax = yMin + (box_size.y - 1);
      break;
    default:
      MINOR_ERROR("Invalid Direction passed into createRoom");
      return false;
    }

    if ((get_map().is_in_bounds(xMin - 1, yMin - 1)) &&
        (get_map().is_in_bounds(xMax + 1, yMax + 1)))
    {
      bool okay = true;

      // Verify that box and surrounding area are solid walls.
      for (int xCheck = xMin - 1; xCheck <= xMax + 1; ++xCheck)
      {
         for (int yCheck = yMin - 1; yCheck <= yMax + 1; ++yCheck)
         {
           MapTile& tile = get_map().get_tile(xCheck, yCheck);
           if (tile.is_empty_space())
           {
             okay = false;
             break;
           }
         }
         if (okay == false) break;
      }

      if (okay)
      {
        // Clear out the box.
        for (int xCoord = xMin; xCoord <= xMax; ++xCoord)
        {
           for (int yCoord = yMin; yCoord <= yMax; ++yCoord)
           {
             MapTile& tile = get_map().get_tile(xCoord, yCoord);
             tile.set_type(MapTileType::FloorStone);
           }
        }

        set_coords(sf::IntRect(xMin, yMin, box_size.x, box_size.y));

        // Add the surrounding walls as potential connection points.
        // First the horizontal walls...
        for (int xCoord = xMin; xCoord <= xMax; ++xCoord)
        {
          add_growth_vector(GeoVector(xCoord, yMin - 1, Direction::North));
          add_growth_vector(GeoVector(xCoord, yMax + 1, Direction::South));
        }
        // Now the vertical walls.
        for (int yCoord = yMin; yCoord <= yMax; ++yCoord)
        {
          add_growth_vector(GeoVector(xMin - 1, yCoord, Direction::West));
          add_growth_vector(GeoVector(xMax + 1, yCoord, Direction::East));
        }

        // TODO: Put either a door or an open area at the starting coords.
        // Right now we just make it an open area.
        MapTile& startTile = get_map().get_tile(startingCoords.x,
                                                   startingCoords.y);
        startTile.set_type(MapTileType::FloorStone);

        // TESTING CODE: 20% chance of having wall sconces in the corners.
        uniform_int_dist lightChanceDist(0, 4);
        int lightChance = lightChanceDist(the_RNG);
        if (lightChance == 0)
        {
          TRACE("Placing some sconces!");

          ThingId neTile = get_map().get_tile_id(xMin, yMin);
          ThingId nwTile = get_map().get_tile_id(xMax, yMin);
          ThingId seTile = get_map().get_tile_id(xMin, yMax);
          ThingId swTile = get_map().get_tile_id(xMax, yMax);

          ThingId neSconce = TF.create(typeid(Sconce).name());
          ThingId nwSconce = TF.create(typeid(Sconce).name());
          ThingId seSconce = TF.create(typeid(Sconce).name());
          ThingId swSconce = TF.create(typeid(Sconce).name());

          TF.get(neSconce).move_into(neTile);
          TF.get(nwSconce).move_into(nwTile);
          TF.get(seSconce).move_into(seTile);
          TF.get(swSconce).move_into(swTile);
        }

        return true;
      }
    }

    ++numTries;
  }

  return false;
}
