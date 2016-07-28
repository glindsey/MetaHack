#include "stdafx.h"

#include "MapRoom.h"

#include "App.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "ThingManager.h"

// Static declarations
/// @todo These should be passed in as arguments
unsigned int MapRoom::max_width = 15;
unsigned int MapRoom::min_width = 3;
unsigned int MapRoom::max_height = 15;
unsigned int MapRoom::min_height = 3;
unsigned int MapRoom::max_retries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

MapRoom::MapRoom(Map& m, PropertyDictionary const& s, GeoVector vec)
  :
  MapFeature{ m, s, vec }
{
  unsigned int num_tries = 0;

  uniform_int_dist width_dist(min_width, max_width);
  uniform_int_dist height_dist(min_height, max_height);

  sf::Vector2i& starting_coords = vec.start_point;
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
    else if (direction == Direction::Self)
    {
      uniform_int_dist height_offset_dist(0, rect.height - 1);
      uniform_int_dist width_offset_dist(0, rect.width - 1);

      rect.top = starting_coords.y - height_offset_dist(the_RNG);
      rect.left = starting_coords.x - width_offset_dist(the_RNG);
    }
    else
    {
      throw MapFeatureException("Invalid direction passed to MapRoom constructor");
    }

    if ((get_map().is_in_bounds(rect.left - 1,
                                rect.top - 1)) &&
                                (get_map().is_in_bounds(rect.left + rect.width,
                                                        rect.top + rect.height)))
    {
      bool okay = true;

      // Verify that box and surrounding area are solid walls.
      okay = does_box_pass_criterion({ rect.left - 1, rect.top - 1 },
      { rect.left + rect.width, rect.top + rect.height },
                                     [&](MapTile& tile) { return !tile.is_empty_space(); });

      if (okay)
      {
        // Clear out the box.
        set_box(rect, "MTFloorDirt");
        set_coords(rect);

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

        /// @todo Put either a door or an open area at the starting coords.
        ///       Right now we just make it an open area.
        auto& startTile = get_map().get_tile(starting_coords.x,
                                             starting_coords.y);
        startTile.set_tile_type("MTFloorDirt");

        return;
      }
    }

    ++num_tries;
  }

  throw MapFeatureException("Out of tries attempting to make MapLRoom");
}