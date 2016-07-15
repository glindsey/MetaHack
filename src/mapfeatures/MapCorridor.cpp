#include "stdafx.h"

#include "MapCorridor.h"

#include "App.h"
#include "MapTile.h"

// Static declarations
/// @todo These should be passed in as arguments
unsigned int MapCorridor::maxLength = 48;
unsigned int MapCorridor::minLength = 3;
unsigned int MapCorridor::maxRetries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapCorridor::Impl
{
  sf::Vector2i endingCoords;
};

MapCorridor::MapCorridor(Map& m, PropertyDictionary const& s)
  :
  MapFeature{ m, s },
  pImpl(NEW Impl())
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

    if (direction == Direction::North)
    {
      yMax = startingCoords.y - 1;
      yMin = yMax - (corridorLen - 1);
      xMin = startingCoords.x;
      xMax = startingCoords.x;
      pImpl->endingCoords.x = startingCoords.x;
      pImpl->endingCoords.y = yMin - 1;
    }
    else if (direction == Direction::South)
    {
      yMin = startingCoords.y + 1;
      yMax = yMin + (corridorLen - 1);
      xMin = startingCoords.x;
      xMax = startingCoords.x;
      pImpl->endingCoords.x = startingCoords.x;
      pImpl->endingCoords.y = yMax + 1;
    }
    else if (direction == Direction::West)
    {
      xMax = startingCoords.x - 1;
      xMin = xMax - (corridorLen - 1);
      yMin = startingCoords.y;
      yMax = startingCoords.y;
      pImpl->endingCoords.x = xMin - 1;
      pImpl->endingCoords.y = startingCoords.y;
    }
    else if (direction == Direction::East)
    {
      xMin = startingCoords.x + 1;
      xMax = xMin + (corridorLen - 1);
      yMin = startingCoords.y;
      yMax = startingCoords.y;
      pImpl->endingCoords.x = xMax + 1;
      pImpl->endingCoords.y = startingCoords.y;
    }
    else
    {
      CLOG(WARNING, "MapGenerator") << "Invalid Direction passed into createCorridor";
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
          auto& tile = get_map().get_tile(xCheck, yCheck);
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
            auto& tile = get_map().get_tile(xCheck, yCheck);
            tile.set_tile_type("MTFloorDirt");
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
        auto& startTile = get_map().get_tile(startingCoords.x,
                                             startingCoords.y);
        startTile.set_tile_type("MTFloorDirt");

        /// Check the tile two past the ending tile.
        /// If it is open space, there should be a small chance
        /// (maybe 5%) that the corridor opens up into it.  This
        /// will allow for some loops in the map instead of it being
        /// nothing but a tree.
        sf::Vector2i checkCoords;
        if (direction == Direction::North)
        {
          checkCoords.x = startingCoords.x;
          checkCoords.y = pImpl->endingCoords.y - 1;
        }
        else if (direction == Direction::South)
        {
          checkCoords.x = startingCoords.x;
          checkCoords.y = pImpl->endingCoords.y + 1;
        }
        else if (direction == Direction::West)
        {
          checkCoords.x = pImpl->endingCoords.x - 1;
          checkCoords.y = startingCoords.y;
        }
        else if (direction == Direction::East)
        {
          checkCoords.x = pImpl->endingCoords.x + 1;
          checkCoords.y = startingCoords.y;
        }
        else
        {
          CLOG(WARNING, "MapGenerator") << "Invalid Direction passed into createCorridor";
          return false;
        }

        if (get_map().is_in_bounds(checkCoords.x, checkCoords.y))
        {
          auto& checkTile = get_map().get_tile(checkCoords.x, checkCoords.y);
          if (checkTile.is_empty_space())
          {
            auto& endTile = get_map().get_tile(pImpl->endingCoords.x,
                                               pImpl->endingCoords.y);
            endTile.set_tile_type("MTFloorDirt");

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
  return pImpl->endingCoords;
}