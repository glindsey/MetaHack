#include "stdafx.h"

#include "map/MapDonutRoom.h"

#include "entity/EntityPool.h"
#include "game/App.h"
#include "maptile/MapTile.h"
#include "utilities/MathUtils.h"

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

MapDonutRoom::MapDonutRoom(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  MapFeature{ m, s, vec }
{
  unsigned int num_tries = 0;

  uniform_int_dist width_dist(s.get("min_width", 7),
                              s.get("max_width", 20));
  uniform_int_dist height_dist(s.get("min_height", 7),
                               s.get("max_height", 20));
  unsigned int min_hole_size = s.get("min_hole_size", 5);
  unsigned int max_retries = s.get("max_retries", 500);
  std::string floor_type = s.get("floor_type", "MTFloorDirt");
  

  IntVec2& starting_coords = vec.start_point;
  Direction& direction = vec.direction;

  while (num_tries < max_retries)
  {
    sf::IntRect rect;

    rect.width = width_dist(the_RNG);
    rect.height = height_dist(the_RNG);

    if (direction == Direction::North)
    {
      uniform_int_dist offset_dist(0, rect.width - 1);
      int offset = offset_dist(the_RNG);

      rect.top = starting_coords.y - rect.height;
      rect.left = starting_coords.x - offset;
    }
    else if (direction == Direction::South)
    {
      uniform_int_dist offset_dist(0, rect.width - 1);
      int offset = offset_dist(the_RNG);

      rect.top = starting_coords.y + 1;
      rect.left = starting_coords.x - offset;
    }
    else if (direction == Direction::West)
    {
      uniform_int_dist offset_dist(0, rect.height - 1);
      int offset = offset_dist(the_RNG);

      rect.top = starting_coords.y - offset;
      rect.left = starting_coords.x - rect.width;
    }
    else if (direction == Direction::East)
    {
      uniform_int_dist offset_dist(0, rect.height - 1);
      int offset = offset_dist(the_RNG);

      rect.top = starting_coords.y - offset;
      rect.left = starting_coords.x + 1;
    }
    else
    {
      throw MapFeatureException("Invalid direction passed to MapDonutRoom constructor");
    }

    if ((getMap().isInBounds({ rect.left - 1, rect.top - 1 })) &&
      (getMap().isInBounds({ rect.left + rect.width, rect.top + rect.height })))
    {
      bool okay = true;

      okay = does_box_pass_criterion({ rect.left - 1, rect.top - 1 },
      { rect.left + rect.width, rect.top + rect.height },
                                     [&](MapTile& tile) { return !tile.isEmptySpace(); });

      // Create the hole location.
      sf::IntRect hole;
      uniform_int_dist hole_x_dist(rect.left + 1, rect.left + rect.width - 2);
      uniform_int_dist hole_y_dist(rect.top + 1, rect.top + rect.height - 2);

      int x_hole_left = hole_x_dist(the_RNG);
      int x_hole_right = hole_x_dist(the_RNG);
      int y_hole_top = hole_y_dist(the_RNG);
      int y_hole_bottom = hole_y_dist(the_RNG);

      // Make sure the hole isn't TOO small.
      // GSL GRUMBLE: WHY does abs() return a signed value?!?
      if ((static_cast<unsigned int>(abs(x_hole_right - x_hole_left)) < min_hole_size - 1) ||
        (static_cast<unsigned int>(abs(y_hole_bottom - y_hole_top)) < min_hole_size - 1))
      {
        okay = false;
      }

      if (x_hole_right < x_hole_left) std::swap(x_hole_left, x_hole_right);
      if (y_hole_bottom < y_hole_top) std::swap(y_hole_top, y_hole_bottom);

      if (okay)
      {
        // Clear out the box EXCEPT FOR the hole.
        for (int x_coord = rect.left;
             x_coord <= rect.left + rect.width - 1;
             ++x_coord)
        {
          for (int y_coord = rect.top;
               y_coord <= rect.top + rect.height - 1;
               ++y_coord)
          {
            if (!((x_coord >= x_hole_left) && (x_coord <= x_hole_right) &&
              (y_coord >= y_hole_top) && (y_coord <= y_hole_bottom)))
            {
              auto& tile = getMap().getTile({ x_coord, y_coord });
              tile.setTileType(floor_type);
            }
          }
        }

        setCoords(rect);

        // Add the surrounding walls as potential connection points.

        // Horizontal walls...
        for (int x_coord = rect.left + 1;
             x_coord <= rect.left + rect.width - 1;
             ++x_coord)
        {
          add_growth_vector(GeoVector(x_coord, rect.top - 1, Direction::North));
          add_growth_vector(GeoVector(x_coord, rect.top + rect.height, Direction::South));
        }
        // Vertical walls...
        for (int y_coord = rect.top + 1;
             y_coord <= rect.top + rect.height - 1;
             ++y_coord)
        {
          add_growth_vector(GeoVector(rect.left - 1, y_coord, Direction::West));
          add_growth_vector(GeoVector(rect.left + rect.width, y_coord, Direction::East));
        }

        // Do the same for the hole walls.
        // Horizontal walls...
        for (int x_coord = x_hole_left + 1; x_coord < x_hole_right; ++x_coord)
        {
          add_growth_vector(GeoVector(x_coord, y_hole_bottom, Direction::North));
          add_growth_vector(GeoVector(x_coord, y_hole_top, Direction::South));
        }
        // Vertical walls...
        for (int y_coord = y_hole_top + 1; y_coord < y_hole_bottom; ++y_coord)
        {
          add_growth_vector(GeoVector(x_hole_right, y_coord, Direction::West));
          add_growth_vector(GeoVector(x_hole_left, y_coord, Direction::East));
        }

        /// @todo Put either a door or an open area at the starting coords.
        ///       Right now we just make it an open area.
        auto& startTile = getMap().getTile(starting_coords);
        startTile.setTileType(floor_type);

        return;
      }
    }

    ++num_tries;
  }

  throw MapFeatureException("Out of tries attempting to make MapDonutRoom");
}