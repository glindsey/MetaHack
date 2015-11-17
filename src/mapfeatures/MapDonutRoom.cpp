#include "MapDonutRoom.h"

#include <boost/log/trivial.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "ThingManager.h"

// Static declarations
unsigned int MapDonutRoom::max_width = 20;
unsigned int MapDonutRoom::min_width = 7;
unsigned int MapDonutRoom::max_height = 20;
unsigned int MapDonutRoom::min_height = 7;
unsigned int MapDonutRoom::min_hole_size = 5;
unsigned int MapDonutRoom::max_retries = 500;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

MapDonutRoom::MapDonutRoom(Map& m, PropertyDictionary const& s) 
  :
  MapFeature{ m, s }
{
  //ctor
}

MapDonutRoom::~MapDonutRoom()
{
  //dtor
}

bool MapDonutRoom::create(GeoVector vec)
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

    switch (direction)
    {
    case Direction::North:
      {
        uniform_int_dist offset_dist(0, rect.width - 1);
        int offset = offset_dist(the_RNG);

        rect.top = starting_coords.y - rect.height;
        rect.left = starting_coords.x - offset;
      }
      break;
    case Direction::South:
      {
        uniform_int_dist offset_dist(0, rect.width - 1);
        int offset = offset_dist(the_RNG);

        rect.top = starting_coords.y + 1;
        rect.left = starting_coords.x - offset;
      }
      break;
    case Direction::West:
      {
        uniform_int_dist offset_dist(0, rect.height - 1);
        int offset = offset_dist(the_RNG);

        rect.top = starting_coords.y - offset;
        rect.left = starting_coords.x - rect.width;
      }
      break;
    case Direction::East:
      {
        uniform_int_dist offset_dist(0, rect.height - 1);
        int offset = offset_dist(the_RNG);

        rect.top = starting_coords.y - offset;
        rect.left = starting_coords.x + 1;
      }
      break;
    default:
      MINOR_ERROR("Invalid direction");
      return false;
    }

    if ((get_map().is_in_bounds(rect.left - 1,
                                rect.top - 1)) &&
        (get_map().is_in_bounds(rect.left + rect.width,
                                rect.top + rect.height)))
    {
      bool okay = true;

      // Verify that box and surrounding area are solid walls.
      for (int x_check = rect.left - 1;
               x_check <= rect.left + rect.width;
               ++x_check)
      {
         for (int y_check = rect.top - 1;
                  y_check <= rect.top + rect.height;
                  ++y_check)
         {
           auto& tile = get_map().get_tile(x_check, y_check);
           if (tile.is_empty_space())
           {
             okay = false;
             break;
           }
         }
         if (okay == false) break;
      }

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
               auto& tile = get_map().get_tile(x_coord, y_coord);
               tile.set_type("MTFloorDirt");
             }
           }
        }

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
        auto& startTile = get_map().get_tile(starting_coords.x,
                                             starting_coords.y);
        startTile.set_type("MTFloorDirt");

        return true;
      }
    }

    ++num_tries;
  }

  return false;
}
