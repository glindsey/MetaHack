#include "stdafx.h"

#include "MapDiamond.h"

#include "App.h"
#include "MapTile.h"
#include "ThingManager.h"

// Static declarations
/// @todo These should be passed in as arguments
unsigned int MapDiamond::maxHalfSize = 4;
unsigned int MapDiamond::minHalfSize = 2;
unsigned int MapDiamond::maxRetries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapDiamond::Impl
{
  bool dummy;
};

MapDiamond::MapDiamond(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  MapFeature{ m, s, vec },
  pImpl(NEW Impl())
{
  unsigned int numTries = 0;
  uniform_int_dist hsDist(minHalfSize, maxHalfSize);

  Vec2i& startingCoords = vec.start_point;
  Direction& direction = vec.direction;

  while (numTries < maxRetries)
  {
    int diamondHalfSize = hsDist(the_RNG);

    int xCenter, yCenter;

    if (direction == Direction::North)
    {
      xCenter = startingCoords.x;
      yCenter = startingCoords.y - (diamondHalfSize + 1);
    }
    else if (direction == Direction::South)
    {
      xCenter = startingCoords.x;
      yCenter = startingCoords.y + (diamondHalfSize + 1);
    }
    else if (direction == Direction::West)
    {
      xCenter = startingCoords.x - (diamondHalfSize + 1);
      yCenter = startingCoords.y;
    }
    else if (direction == Direction::East)
    {
      xCenter = startingCoords.x + (diamondHalfSize + 1);
      yCenter = startingCoords.y;
    }
    else if (direction == Direction::Self)
    {
      xCenter = startingCoords.x;
      yCenter = startingCoords.y;
    }
    else
    {
      throw MapFeatureException("Invalid direction passed to MapDiamond constructor");
    }

    if ((get_map().is_in_bounds({ xCenter - (diamondHalfSize + 1), yCenter - (diamondHalfSize + 1) })) &&
        (get_map().is_in_bounds({ xCenter + (diamondHalfSize + 1), yCenter + (diamondHalfSize + 1) })))
    {
      bool okay = true;

      // Verify that box and surrounding area are solid walls.
      /// @todo: Constrain this to only check around the edges of the
      ///        diamond, instead of the entire enclosing box.

      okay = does_box_pass_criterion({ xCenter - (diamondHalfSize + 1), yCenter - (diamondHalfSize + 1) },
      { xCenter + (diamondHalfSize + 1), yCenter + (diamondHalfSize + 1) },
                                     [&](MapTile& tile) { return !tile.is_empty_space(); });

      if (okay)
      {
        // Clear out a diamond.
        for (int xCounter = -diamondHalfSize;
             xCounter <= diamondHalfSize;
             ++xCounter)
        {
          for (int yCounter = -(diamondHalfSize - abs(xCounter));
               yCounter <= diamondHalfSize - abs(xCounter);
               ++yCounter)
          {
            int xCoord = xCenter + xCounter;
            int yCoord = yCenter + yCounter;
            auto& tile = get_map().get_tile({ xCoord, yCoord });
            tile.set_tile_type("MTFloorDirt");
          }
        }

        set_coords(sf::IntRect(xCenter - diamondHalfSize,
                               yCenter - diamondHalfSize,
                               (diamondHalfSize * 2) + 1,
                               (diamondHalfSize * 2) + 1));

        // Add the four points as potential connection points.
        add_growth_vector(GeoVector(xCenter, yCenter - (diamondHalfSize + 1),
                                    Direction::North));
        add_growth_vector(GeoVector(xCenter, yCenter + (diamondHalfSize + 1),
                                    Direction::South));
        add_growth_vector(GeoVector(xCenter - (diamondHalfSize + 1), yCenter,
                                    Direction::West));
        add_growth_vector(GeoVector(xCenter + (diamondHalfSize + 1), yCenter,
                                    Direction::East));

        /// @todo Put either a door or an open area at the starting coords.
        ///       Right now we just make it an open area.
        auto& startTile = get_map().get_tile(startingCoords);
        startTile.set_tile_type("MTFloorDirt");

        return;
      }
    }

    ++numTries;
  }

  throw MapFeatureException("Out of tries attempting to make MapDiamond");
}