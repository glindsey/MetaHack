#include "mapfeatures/MapCorridor.h"

#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "ErrorHandler.h"
#include "MapTile.h"

// Static declarations
unsigned int MapCorridor::maxLength = 48;
unsigned int MapCorridor::minLength = 3;
unsigned int MapCorridor::maxRetries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapCorridor::Impl
{
  sf::Vector2i endingCoords;
};

MapCorridor::MapCorridor(Map& m)
  : MapFeature(m), impl(new Impl())
{
  //ctor
}

MapCorridor::~MapCorridor()
{
  //dtor
}

bool MapCorridor::create(GeoVector vec)
{
    unsigned int numTries = 0;
    uniform_int_dist lenDist(minLength, maxLength);

    sf::Vector2i& startingCoords = vec.start_point;
    Direction& direction = vec.direction;

    while (numTries < maxRetries)
    {
      int corridorLen(lenDist(the_RNG));

      int xMin, xMax, yMin, yMax;

      switch (direction)
      {
      case Direction::North:
        yMax = startingCoords.y - 1;
        yMin = yMax - (corridorLen - 1);
        xMin = startingCoords.x;
        xMax = startingCoords.x;
        impl->endingCoords.x = startingCoords.x;
        impl->endingCoords.y = yMin - 1;
        break;
      case Direction::South:
        yMin = startingCoords.y + 1;
        yMax = yMin + (corridorLen - 1);
        xMin = startingCoords.x;
        xMax = startingCoords.x;
        impl->endingCoords.x = startingCoords.x;
        impl->endingCoords.y = yMax + 1;
        break;
      case Direction::West:
        xMax = startingCoords.x - 1;
        xMin = xMax - (corridorLen - 1);
        yMin = startingCoords.y;
        yMax = startingCoords.y;
        impl->endingCoords.x = xMin - 1;
        impl->endingCoords.y = startingCoords.y;
        break;
      case Direction::East:
        xMin = startingCoords.x + 1;
        xMax = xMin + (corridorLen - 1);
        yMin = startingCoords.y;
        yMax = startingCoords.y;
        impl->endingCoords.x = xMax + 1;
        impl->endingCoords.y = startingCoords.y;
        break;
      default:
        MINOR_ERROR("Invalid Direction passed into createCorridor");
        return false;
      }

      if ((get_map().is_in_bounds(xMin - 1, yMin - 1)) &&
          (get_map().is_in_bounds(xMax + 1, yMax + 1)))
      {
        bool okay = true;

        // Verify that corridor and surrounding area are solid walls.
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
          for (int xCheck = xMin; xCheck <= xMax; ++xCheck)
          {
             for (int yCheck = yMin; yCheck <= yMax; ++yCheck)
             {
               MapTile& tile = get_map().get_tile(xCheck, yCheck);
               tile.set_type(MapTileType::FloorStone);
             }
          }

          set_coords(sf::IntRect(xMin, yMin,
                                (xMax - xMin) + 1,
                                (yMax - yMin) + 1));

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

          /// @todo: Put either a door or an open area at the starting coords.
          ///        Right now we just make it an open area.
          MapTile& startTile = get_map().get_tile(startingCoords.x,
                                                startingCoords.y);
          startTile.set_type(MapTileType::FloorStone);

          /// Check the tile two past the ending tile.
          /// If it is open space, there should be a small chance
          /// (maybe 5%) that the corridor opens up into it.  This
          /// will allow for some loops in the map instead of it being
          /// nothing but a tree.
          sf::Vector2i checkCoords;
          switch (direction)
          {
          case Direction::North:
            checkCoords.x = startingCoords.x;
            checkCoords.y = impl->endingCoords.y - 1;
            break;
          case Direction::South:
            checkCoords.x = startingCoords.x;
            checkCoords.y = impl->endingCoords.y + 1;
            break;
          case Direction::West:
            checkCoords.x = impl->endingCoords.x - 1;
            checkCoords.y = startingCoords.y;
            break;
          case Direction::East:
            checkCoords.x = impl->endingCoords.x + 1;
            checkCoords.y = startingCoords.y;
            break;
          default:
            // shouldn't happen at this point
            MINOR_ERROR("Invalid Direction passed into createCorridor");
            return false;
          }
          if (get_map().is_in_bounds(checkCoords.x, checkCoords.y))
          {
            MapTile& checkTile = get_map().get_tile(checkCoords.x, checkCoords.y);
            if (checkTile.is_empty_space())
            {
              /// @todo Keep going here
            }
          }

          return true;
        }
      }

      ++numTries;
    }

    return false;
  }

sf::Vector2i const& MapCorridor::getEndingCoords() const
{
  return impl->endingCoords;
}
