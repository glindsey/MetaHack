#include "stdafx.h"

#include "map/MapCorridor.h"

#include "game/App.h"
#include "maptile/MapTile.h"
#include "utilities/RNGUtils.h"

MapCorridor::MapCorridor(Map& m, PropertyDictionary const& s, GeoVector vec)
  : MapFeature{ m, s, vec }
{
  unsigned int numTries = 0;
  UniformIntDist lenDist(s.get("min_length", 3), 
                           s.get("max_length", 48));
  unsigned int max_retries = s.get("max_retries", 100);
  std::string floor_type = s.get("floor_type", "MTFloorDirt");

  IntVec2& startingCoords = vec.start_point;
  Direction& direction = vec.direction;

  while (numTries < max_retries)
  {
    int corridorLen(lenDist(the_RNG));

    int xMin, xMax, yMin, yMax;

    if (direction == Direction::North)
    {
      yMax = startingCoords.y - 1;
      yMin = yMax - (corridorLen - 1);
      xMin = startingCoords.x;
      xMax = startingCoords.x;
      m_endingCoords.x = startingCoords.x;
      m_endingCoords.y = yMin - 1;
    }
    else if (direction == Direction::South)
    {
      yMin = startingCoords.y + 1;
      yMax = yMin + (corridorLen - 1);
      xMin = startingCoords.x;
      xMax = startingCoords.x;
      m_endingCoords.x = startingCoords.x;
      m_endingCoords.y = yMax + 1;
    }
    else if (direction == Direction::West)
    {
      xMax = startingCoords.x - 1;
      xMin = xMax - (corridorLen - 1);
      yMin = startingCoords.y;
      yMax = startingCoords.y;
      m_endingCoords.x = xMin - 1;
      m_endingCoords.y = startingCoords.y;
    }
    else if (direction == Direction::East)
    {
      xMin = startingCoords.x + 1;
      xMax = xMin + (corridorLen - 1);
      yMin = startingCoords.y;
      yMax = startingCoords.y;
      m_endingCoords.x = xMax + 1;
      m_endingCoords.y = startingCoords.y;
    }
    else
    {
      throw MapFeatureException("Invalid direction passed to MapCorridor constructor");
    }

    if ((getMap().isInBounds({ xMin - 1, yMin - 1 })) &&
      (getMap().isInBounds({ xMax + 1, yMax + 1 })))
    {
      bool okay = true;

      // Verify that corridor and surrounding area are solid walls.
      okay = does_box_pass_criterion(
      { xMin - 1, yMin - 1 },
      { xMax + 1, yMax + 1 },
                                     [&](MapTile& tile) { return !tile.isPassable(); });

      if (okay)
      {
        // Clear out the box.
        set_box({ xMin, yMin }, { xMax, yMax }, floor_type);

        setCoords(sf::IntRect(xMin, yMin, (xMax - xMin) + 1, (yMax - yMin) + 1));

        // Add the surrounding walls as potential connection points.
        // First the horizontal walls...
        for (int xCoord = xMin; xCoord <= xMax; ++xCoord)
        {
          // If a vertical corridor, horizontal walls are high priority connections.
          bool priority = ((direction == Direction::North) || (direction == Direction::South));
          add_growth_vector(GeoVector(xCoord, yMin - 1, Direction::North), priority);
          add_growth_vector(GeoVector(xCoord, yMax + 1, Direction::South), priority);
        }
        // Now the vertical walls.
        for (int yCoord = yMin; yCoord <= yMax; ++yCoord)
        {
          // If a horizontal corridor, vertical walls are high priority connections.
          bool priority = ((direction == Direction::East) || (direction == Direction::West));
          add_growth_vector(GeoVector(xMin - 1, yCoord, Direction::West), priority);
          add_growth_vector(GeoVector(xMax + 1, yCoord, Direction::East), priority);
        }

        /// @todo: Put either a door or an open area at the starting coords.
        ///        Right now we just make it an open area.
        auto& startTile = getMap().getTile(startingCoords);
        startTile.setTileType(floor_type);

        /// Check the tile two past the ending tile.
        /// If it is open space, there should be a small chance
        /// (maybe 5%) that the corridor opens up into it.  This
        /// will allow for some loops in the map instead of it being
        /// nothing but a tree.
        IntVec2 checkCoords;
        if (direction == Direction::North)
        {
          checkCoords.x = startingCoords.x;
          checkCoords.y = m_endingCoords.y - 1;
        }
        else if (direction == Direction::South)
        {
          checkCoords.x = startingCoords.x;
          checkCoords.y = m_endingCoords.y + 1;
        }
        else if (direction == Direction::West)
        {
          checkCoords.x = m_endingCoords.x - 1;
          checkCoords.y = startingCoords.y;
        }
        else if (direction == Direction::East)
        {
          checkCoords.x = m_endingCoords.x + 1;
          checkCoords.y = startingCoords.y;
        }
        else
        {
          throw MapFeatureException("Invalid direction passed to MapCorridor constructor");
        }

        if (getMap().isInBounds(checkCoords))
        {
          auto& checkTile = getMap().getTile(checkCoords);
          if (checkTile.isPassable())
          {
            /// @todo Do a throw to see if it opens up. Right now it always does.
            auto& endTile = getMap().getTile(m_endingCoords);
            endTile.setTileType(floor_type);
          }
        }

        return;
      }
    }

    ++numTries;
  }

  throw MapFeatureException("Out of tries attempting to make MapCorridor");
}

IntVec2 const& MapCorridor::getEndingCoords() const
{
  return m_endingCoords;
}