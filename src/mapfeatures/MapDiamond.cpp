#include "mapfeatures/MapDiamond.h"

#include <boost/log/trivial.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "MapTile.h"
#include "ThingManager.h"

// Static declarations
unsigned int MapDiamond::maxHalfSize = 4;
unsigned int MapDiamond::minHalfSize = 2;
unsigned int MapDiamond::maxRetries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct MapDiamond::Impl
{
  bool dummy;
};

MapDiamond::MapDiamond(Map& m)
  : MapFeature(m), pImpl(new Impl())
{
  //ctor
}

MapDiamond::~MapDiamond()
{
  //dtor
}

bool MapDiamond::create(GeoVector vec)
{
  unsigned int numTries = 0;
  uniform_int_dist hsDist(minHalfSize, maxHalfSize);

  sf::Vector2i& startingCoords = vec.start_point;
  Direction& direction = vec.direction;

  while (numTries < maxRetries)
  {
    int diamondHalfSize = hsDist(the_RNG);

    int xCenter, yCenter;

    switch (direction)
    {
    case Direction::North:
      xCenter = startingCoords.x;
      yCenter = startingCoords.y - (diamondHalfSize + 1);
      break;
    case Direction::South:
      xCenter = startingCoords.x;
      yCenter = startingCoords.y + (diamondHalfSize + 1);
      break;
    case Direction::West:
      xCenter = startingCoords.x - (diamondHalfSize + 1);
      yCenter = startingCoords.y;
      break;
    case Direction::East:
      xCenter = startingCoords.x + (diamondHalfSize + 1);
      yCenter = startingCoords.y;
      break;
    case Direction::Self:
      xCenter = startingCoords.x;
      yCenter = startingCoords.y;
      break;
    default:
      MINOR_ERROR("Invalid Direction passed into createRoom");
      return false;
    }

    if ((get_map().is_in_bounds(xCenter - (diamondHalfSize + 1),
                           yCenter - (diamondHalfSize + 1))) &&
        (get_map().is_in_bounds(xCenter + (diamondHalfSize + 1),
                           yCenter + (diamondHalfSize + 1))))
    {
      bool okay = true;

      // Verify that box and surrounding area are solid walls.
      /// @todo: Constrain this to only check around the edges of the
      ///        diamond, instead of the entire enclosing box.
      for (int xCheck = xCenter - (diamondHalfSize + 1);
               xCheck <= xCenter + (diamondHalfSize + 1); ++xCheck)
      {
         for (int yCheck = yCenter - (diamondHalfSize + 1);
                  yCheck <= yCenter + (diamondHalfSize + 1); ++yCheck)
         {
           auto& tile = get_map().get_tile(xCheck, yCheck);
           if (tile->is_empty_space())
           {
             okay = false;
             break;
           }
         }
         if (okay == false) break;
      }

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
            auto& tile = get_map().get_tile(xCoord, yCoord);
            tile->set_type("FloorStone");
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
        auto& startTile = get_map().get_tile(startingCoords.x,
                                             startingCoords.y);
        startTile->set_type("FloorStone");

        return true;
      }
    }

    ++numTries;
  }

  return false;
}
