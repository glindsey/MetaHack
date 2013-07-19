#include "MapRoom.h"

#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "ErrorHandler.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "ThingFactory.h"

// Static declarations
unsigned int MapRoom::max_width = 15;
unsigned int MapRoom::min_width = 3;
unsigned int MapRoom::max_height = 15;
unsigned int MapRoom::min_height = 3;
unsigned int MapRoom::max_retries = 100;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

MapRoom::MapRoom(Map& m) :
  MapFeature(m)
{
  //ctor
}

MapRoom::~MapRoom()
{
  //dtor
}

bool MapRoom::create(GeoVector vec)
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
    case Direction::Self:
      {
        uniform_int_dist height_offset_dist(0, rect.height - 1);
        uniform_int_dist width_offset_dist(0, rect.width - 1);

        rect.top = starting_coords.y - height_offset_dist(the_RNG);
        rect.left = starting_coords.x - width_offset_dist(the_RNG);
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
           MapTile& tile = get_map().get_tile(x_check, y_check);
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
        for (int x_coord = rect.left;
                 x_coord <= rect.left + rect.width - 1;
                 ++x_coord)
        {
           for (int y_coord = rect.top;
                    y_coord <= rect.top + rect.height - 1;
                    ++y_coord)
           {
             MapTile& tile = get_map().get_tile(x_coord, y_coord);
             tile.set_type(MapTileType::FloorStone);
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

        /// @todo Put either a door or an open area at the starting coords.
        ///       Right now we just make it an open area.
        MapTile& startTile = get_map().get_tile(starting_coords.x,
                                                   starting_coords.y);
        startTile.set_type(MapTileType::FloorStone);

        return true;
      }
    }

    ++num_tries;
  }

  return false;
}
