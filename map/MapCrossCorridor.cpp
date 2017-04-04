#include "stdafx.h"

#include "MapCrossCorridor.h"

#include "game/App.h"
#include "map/MapTile.h"
#include "utilities/JSONHelpers.h"

// Static declarations
/// @todo These should be passed in as arguments
unsigned int MapCrossCorridor::maxLength = 48;
unsigned int MapCrossCorridor::minLength = 3;
unsigned int MapCrossCorridor::maxRetries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapCrossCorridor::Impl
{
  sf::Vector2i endingCoords;
};

MapCrossCorridor::MapCrossCorridor(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  MapFeature{ m, s, vec },
  pImpl(NEW Impl())
{
  unsigned int numTries = 0;
  uniform_int_dist lenDist(minLength, maxLength);

  sf::Vector2i& startingCoords = vec.start_point;
  Direction& direction = vec.direction;

  while (numTries < maxRetries)
  {
    int mainCorridorLen(lenDist(the_RNG));
    int subCorridorLen(lenDist(the_RNG));

    int mainXMin, mainXMax, mainYMin, mainYMax;
    int subXMin, subXMax, subYMin, subYMax;

    // Create main corridor bounds.
    if (direction == Direction::North)
    {
      mainYMax = startingCoords.y - 1;
      mainYMin = mainYMax - (mainCorridorLen - 1);
      mainXMin = startingCoords.x;
      mainXMax = startingCoords.x;
      pImpl->endingCoords.x = startingCoords.x;
      pImpl->endingCoords.y = mainYMin - 1;
    }
    else if (direction == Direction::South)
    {
      mainYMin = startingCoords.y + 1;
      mainYMax = mainYMin + (mainCorridorLen - 1);
      mainXMin = startingCoords.x;
      mainXMax = startingCoords.x;
      pImpl->endingCoords.x = startingCoords.x;
      pImpl->endingCoords.y = mainYMax + 1;
    }
    else if (direction == Direction::West)
    {
      mainXMax = startingCoords.x - 1;
      mainXMin = mainXMax - (mainCorridorLen - 1);
      mainYMin = startingCoords.y;
      mainYMax = startingCoords.y;
      pImpl->endingCoords.x = mainXMin - 1;
      pImpl->endingCoords.y = startingCoords.y;
    }
    else if (direction == Direction::East)
    {
      mainXMin = startingCoords.x + 1;
      mainXMax = mainXMin + (mainCorridorLen - 1);
      mainYMin = startingCoords.y;
      mainYMax = startingCoords.y;
      pImpl->endingCoords.x = mainXMax + 1;
      pImpl->endingCoords.y = startingCoords.y;
    }
    else
    {
      throw MapFeatureException("Invalid direction passed to MapCorridor constructor");
    }

    // Create sub corridor bounds.
    if ((direction == Direction::North) || (direction == Direction::South))
    {
      uniform_int_dist subXDist(mainXMin - subCorridorLen, mainXMin + subCorridorLen);
      uniform_int_dist subYDist(mainYMin, mainYMax);

      subXMin = subXDist(the_RNG);
      subXMax = subXMin + (subCorridorLen - 1);
      subYMin = subYDist(the_RNG);
      subYMax = subYMin;
    }
    else if ((direction == Direction::East) || (direction == Direction::West))
    {
      uniform_int_dist subXDist(mainXMin, mainXMax);
      uniform_int_dist subYDist(mainYMin - subCorridorLen, mainYMin + subCorridorLen);

      subXMin = subXDist(the_RNG);
      subXMax = subXMin;
      subYMin = subYDist(the_RNG);
      subYMax = subYMin + (subCorridorLen - 1);

    }

    if ((get_map().is_in_bounds(mainXMin - 1, mainYMin - 1)) &&
        (get_map().is_in_bounds(mainXMax + 1, mainYMax + 1)))
    {
      bool okay = true;

      // Verify that main corridor and surrounding area are solid walls.
      for (int xCheck = mainXMin - 1; xCheck <= mainXMax + 1; ++xCheck)
      {
        for (int yCheck = mainYMin - 1; yCheck <= mainYMax + 1; ++yCheck)
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
        set_box({ mainXMin, mainYMin }, { mainXMax, mainYMax }, "MTFloorDirt");

        set_coords(sf::IntRect(mainXMin, mainYMin,
                               (mainXMax - mainXMin) + 1,
                               (mainYMax - mainYMin) + 1));

        // Add the surrounding walls as potential connection points.
        // First the horizontal walls...
        for (int xCoord = mainXMin; xCoord <= mainXMax; ++xCoord)
        {
          add_growth_vector(GeoVector(xCoord, mainYMin - 1, Direction::North));
          add_growth_vector(GeoVector(xCoord, mainYMax + 1, Direction::South));
        }
        // Now the vertical walls.
        for (int yCoord = mainYMin; yCoord <= mainYMax; ++yCoord)
        {
          add_growth_vector(GeoVector(mainXMin - 1, yCoord, Direction::West));
          add_growth_vector(GeoVector(mainXMax + 1, yCoord, Direction::East));
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
          throw MapFeatureException("Invalid direction passed to MapCrossCorridor constructor");
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

        return;
      }
    }

    ++numTries;
  }

  throw MapFeatureException("Out of tries attempting to make MapCrossCorridor");
}