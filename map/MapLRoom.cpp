#include "stdafx.h"

#include "map/MapLRoom.h"

#include "entity/EntityFactory.h"
#include "game/App.h"
#include "maptile/MapTile.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"

MapLRoom::MapLRoom(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  MapFeature{ m, s, vec }
{
  unsigned int num_tries = 0;

  unsigned int horizLegMinWidth = s.get("horiz_leg_min_width", 10);
  unsigned int horizLegMaxWidth = s.get("horiz_leg_max_width", 20);
  unsigned int horizLegMinHeight = s.get("horiz_leg_min_height", 3);
  unsigned int horizLegMaxHeight = s.get("horiz_leg_max_height", 7);
  unsigned int vertLegMinWidth = s.get("vert_leg_min_width", 10);
  unsigned int vertLegMaxWidth = s.get("vert_leg_max_width", 20);
  unsigned int vertLegMinHeight = s.get("vert_leg_min_height", 3);
  unsigned int vertLegMaxHeight = s.get("vert_leg_max_height", 7);
  unsigned int max_retries = s.get("max_retries", 500);
  std::string floorMaterial = s.get("floor_type", "Dirt");
  std::string wallMaterial = s.get("wall_type", "Stone");

  IntVec2& starting_coords = vec.start_point;
  Direction& direction = vec.direction;

  while (num_tries < max_retries)
  {
    sf::IntRect horiz_rect;
    sf::IntRect vert_rect;

    horiz_rect.width = the_RNG.pick_uniform(horizLegMinWidth, horizLegMaxWidth);
    horiz_rect.height = the_RNG.pick_uniform(horizLegMinHeight, horizLegMaxHeight);
    vert_rect.width = the_RNG.pick_uniform(vertLegMinWidth, vertLegMaxWidth);
    vert_rect.height = the_RNG.pick_uniform(vertLegMinHeight, vertLegMaxHeight);

    if (direction == Direction::North)
    {
      int offset = the_RNG.pick_uniform(0, horiz_rect.width - 1);

      horiz_rect.top = starting_coords.y - horiz_rect.height;
      horiz_rect.left = starting_coords.x - offset;

      vert_rect.top = horiz_rect.top - vert_rect.height;
      vert_rect.left = (the_RNG.flip_coin() ?
                        horiz_rect.left :
                        horiz_rect.left + horiz_rect.width - vert_rect.width);
    }
    else if (direction == Direction::South)
    {
      int offset = the_RNG.pick_uniform(0, horiz_rect.width - 1);

      horiz_rect.top = starting_coords.y + 1;
      horiz_rect.left = starting_coords.x - offset;

      vert_rect.top = horiz_rect.top + horiz_rect.height;
      vert_rect.left = (the_RNG.flip_coin() ?
                        horiz_rect.left :
                        horiz_rect.left + horiz_rect.width - vert_rect.width);
    }
    else if (direction == Direction::West)
    {
      int offset = the_RNG.pick_uniform(0, vert_rect.height - 1);

      vert_rect.top = starting_coords.y - offset;
      vert_rect.left = starting_coords.x - vert_rect.width;

      horiz_rect.top = (the_RNG.flip_coin() ?
                        vert_rect.top :
                        vert_rect.top + vert_rect.height - horiz_rect.height);
      horiz_rect.left = vert_rect.left - horiz_rect.width;
    }
    else if (direction == Direction::East)
    {
      int offset = the_RNG.pick_uniform(0, vert_rect.height - 1);

      vert_rect.top = starting_coords.y - offset;
      vert_rect.left = starting_coords.x + 1;

      horiz_rect.top = (the_RNG.flip_coin() ?
                        vert_rect.top :
                        vert_rect.top + vert_rect.height - horiz_rect.height);
      horiz_rect.left = vert_rect.left + vert_rect.width;
    }
    else
    {
      throw MapFeatureException("Invalid direction passed to MapLRoom constructor");
    }

    if ((getMap().isInBounds({ vert_rect.left - 1, vert_rect.top - 1 })) &&
        (getMap().isInBounds({ vert_rect.left + vert_rect.width, vert_rect.top + vert_rect.height })) &&
        (getMap().isInBounds({ horiz_rect.left - 1,horiz_rect.top - 1 })) &&
        (getMap().isInBounds({ horiz_rect.left + horiz_rect.width, horiz_rect.top + horiz_rect.height })))
    {
      bool okay = true;

      // Verify that both boxes and surrounding area are solid walls.
      okay = doesBoxPassCriterion({ vert_rect.left - 1, vert_rect.top - 1 },
      { vert_rect.left + vert_rect.width, vert_rect.top + vert_rect.height },
                                     [&](MapTile& tile) { return !tile.isPassable(); });

      okay &= doesBoxPassCriterion({ horiz_rect.left - 1, horiz_rect.top - 1 },
      { horiz_rect.left + horiz_rect.width, horiz_rect.top + horiz_rect.height },
                                      [&](MapTile& tile) { return !tile.isPassable(); });

      if (okay)
      {
        // Clear out the boxes.
        setBox(vert_rect, { "Floor", floorMaterial }, { "OpenSpace" });
        setBox(horiz_rect, { "Floor", floorMaterial }, { "OpenSpace" });

        unsigned int x_min = std::min(horiz_rect.left, vert_rect.left);
        unsigned int y_min = std::min(horiz_rect.top, vert_rect.top);
        unsigned int x_max = std::max(horiz_rect.left + horiz_rect.width - 1,
                                      vert_rect.left + vert_rect.width - 1);
        unsigned int y_max = std::max(horiz_rect.top + horiz_rect.height - 1,
                                      vert_rect.top + vert_rect.height - 1);

        setCoords(sf::IntRect(x_min, y_min,
                               x_max - x_min + 1, y_max - y_min + 1));

        // Add the surrounding walls as potential connection points.
        // This leads to some spurious invalid connection points, but that's
        // okay as they'll be discarded when checked for validity.

        // Horizontal rectangle, horizontal walls...
        for (int x_coord = horiz_rect.left + 1;
             x_coord <= horiz_rect.left + horiz_rect.width - 1;
             ++x_coord)
        {
          addGrowthVector(GeoVector(x_coord, horiz_rect.top - 1, Direction::North));
          addGrowthVector(GeoVector(x_coord, horiz_rect.top + horiz_rect.height, Direction::South));
        }
        // Vertical rectangle, horizontal walls...
        for (int x_coord = vert_rect.left + 1;
             x_coord <= vert_rect.left + vert_rect.width - 1;
             ++x_coord)
        {
          addGrowthVector(GeoVector(x_coord, vert_rect.top - 1, Direction::North));
          addGrowthVector(GeoVector(x_coord, vert_rect.top + vert_rect.height, Direction::South));
        }
        // Horizontal rectangle, vertical walls...
        for (int y_coord = horiz_rect.top + 1;
             y_coord <= horiz_rect.top + horiz_rect.height - 1;
             ++y_coord)
        {
          addGrowthVector(GeoVector(horiz_rect.left - 1, y_coord, Direction::West));
          addGrowthVector(GeoVector(horiz_rect.left + horiz_rect.width, y_coord, Direction::East));
        }
        // Vertical rectangle, vertical walls...
        for (int y_coord = vert_rect.top + 1;
             y_coord <= vert_rect.top + vert_rect.height - 1;
             ++y_coord)
        {
          addGrowthVector(GeoVector(vert_rect.left - 1, y_coord, Direction::West));
          addGrowthVector(GeoVector(vert_rect.left + vert_rect.width, y_coord, Direction::East));
        }

        /// @todo Put either a door or an open area at the starting coords.
        ///       Right now we just make it an open area.
        auto& startTile = getMap().getTile(starting_coords);
        startTile.setTileType({ "Floor", floorMaterial }, { "OpenSpace" });

        return;
      }
    }

    ++num_tries;
  }

  throw MapFeatureException("Out of tries attempting to make MapLRoom");
}
