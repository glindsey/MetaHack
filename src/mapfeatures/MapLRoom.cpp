#include "mapfeatures/MapLRoom.h"

#include <boost/log/trivial.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "App.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "ThingManager.h"

// Static declarations
unsigned int MapLRoom::horiz_leg_max_width = 20;
unsigned int MapLRoom::horiz_leg_min_width = 10;
unsigned int MapLRoom::horiz_leg_max_height = 7;
unsigned int MapLRoom::horiz_leg_min_height = 3;
unsigned int MapLRoom::vert_leg_max_width = 7;
unsigned int MapLRoom::vert_leg_min_width = 3;
unsigned int MapLRoom::vert_leg_max_height = 20;
unsigned int MapLRoom::vert_leg_min_height = 10;
unsigned int MapLRoom::max_retries = 500;

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

MapLRoom::MapLRoom(Map& m) :
  MapFeature(m)
{
  //ctor
}

MapLRoom::~MapLRoom()
{
  //dtor
}

bool MapLRoom::create(GeoVector vec)
{
  unsigned int num_tries = 0;

  uniform_int_dist horiz_width_dist(horiz_leg_min_width,
                                    horiz_leg_max_width);
  uniform_int_dist horiz_height_dist(horiz_leg_min_height,
                                     horiz_leg_max_height);
  uniform_int_dist vert_width_dist(vert_leg_min_width,
                                   vert_leg_max_width);
  uniform_int_dist vert_height_dist(vert_leg_min_height,
                                    vert_leg_max_height);

  sf::Vector2i& starting_coords = vec.start_point;
  Direction& direction = vec.direction;

  while (num_tries < max_retries)
  {
    sf::IntRect horiz_rect;
    sf::IntRect vert_rect;

    horiz_rect.width = horiz_width_dist(the_RNG);
    horiz_rect.height = horiz_height_dist(the_RNG);
    vert_rect.width = vert_width_dist(the_RNG);
    vert_rect.height = vert_height_dist(the_RNG);

    switch (direction)
    {
    case Direction::North:
      {
        uniform_int_dist offset_dist(0, horiz_rect.width - 1);
        int offset = offset_dist(the_RNG);

        horiz_rect.top = starting_coords.y - horiz_rect.height;
        horiz_rect.left = starting_coords.x - offset;

        vert_rect.top = horiz_rect.top - vert_rect.height;
        vert_rect.left = (flip_coin() ?
                          horiz_rect.left :
                          horiz_rect.left + horiz_rect.width - vert_rect.width);
      }
      break;
    case Direction::South:
      {
        uniform_int_dist offset_dist(0, horiz_rect.width - 1);
        int offset = offset_dist(the_RNG);

        horiz_rect.top = starting_coords.y + 1;
        horiz_rect.left = starting_coords.x - offset;

        vert_rect.top = horiz_rect.top + horiz_rect.height;
        vert_rect.left = (flip_coin() ?
                          horiz_rect.left :
                          horiz_rect.left + horiz_rect.width - vert_rect.width);
      }
      break;
    case Direction::West:
      {
        uniform_int_dist offset_dist(0, vert_rect.height - 1);
        int offset = offset_dist(the_RNG);

        vert_rect.top = starting_coords.y - offset;
        vert_rect.left = starting_coords.x - vert_rect.width;

        horiz_rect.top = (flip_coin() ?
                          vert_rect.top :
                          vert_rect.top + vert_rect.height - horiz_rect.height);
        horiz_rect.left = vert_rect.left - horiz_rect.width;
      }
      break;
    case Direction::East:
      {
        uniform_int_dist offset_dist(0, vert_rect.height - 1);
        int offset = offset_dist(the_RNG);

        vert_rect.top = starting_coords.y - offset;
        vert_rect.left = starting_coords.x + 1;

        horiz_rect.top = (flip_coin() ?
                          vert_rect.top :
                          vert_rect.top + vert_rect.height - horiz_rect.height);
        horiz_rect.left = vert_rect.left + vert_rect.width;
      }
      break;
    default:
      MINOR_ERROR("Invalid direction");
      return false;
    }

    if ((get_map().is_in_bounds(vert_rect.left - 1,
                                vert_rect.top - 1)) &&
        (get_map().is_in_bounds(vert_rect.left + vert_rect.width,
                                vert_rect.top + vert_rect.height)) &&
        (get_map().is_in_bounds(horiz_rect.left - 1,
                                horiz_rect.top - 1)) &&
        (get_map().is_in_bounds(horiz_rect.left + horiz_rect.width,
                                horiz_rect.top + horiz_rect.height)))
    {
      bool okay = true;

      // Verify that both boxes and surrounding area are solid walls.
      for (int x_check = vert_rect.left - 1;
               x_check <= vert_rect.left + vert_rect.width;
               ++x_check)
      {
         for (int y_check = vert_rect.top - 1;
                  y_check <= vert_rect.top + vert_rect.height;
                  ++y_check)
         {
           auto& tile = get_map().get_tile(x_check, y_check);
           if (tile->is_empty_space())
           {
             okay = false;
             break;
           }
         }
         if (okay == false) break;
      }

      for (int x_check = horiz_rect.left - 1;
               x_check <= horiz_rect.left + horiz_rect.width;
               ++x_check)
      {
         for (int y_check = horiz_rect.top - 1;
                  y_check <= horiz_rect.top + horiz_rect.height;
                  ++y_check)
         {
           auto& tile = get_map().get_tile(x_check, y_check);
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
        // Clear out the boxes.
        for (int x_coord = vert_rect.left;
                 x_coord <= vert_rect.left + vert_rect.width - 1;
                 ++x_coord)
        {
           for (int y_coord = vert_rect.top;
                    y_coord <= vert_rect.top + vert_rect.height - 1;
                    ++y_coord)
           {
             auto& tile = get_map().get_tile(x_coord, y_coord);
             tile->set_type(MapTileType::FloorStone);
           }
        }

        for (int x_coord = horiz_rect.left;
                 x_coord <= horiz_rect.left + horiz_rect.width - 1;
                 ++x_coord)
        {
           for (int y_coord = horiz_rect.top;
                    y_coord <= horiz_rect.top + horiz_rect.height - 1;
                    ++y_coord)
           {
             auto& tile = get_map().get_tile(x_coord, y_coord);
             tile->set_type(MapTileType::FloorStone);
           }
        }

        unsigned int x_min = std::min(horiz_rect.left, vert_rect.left);
        unsigned int y_min = std::min(horiz_rect.top, vert_rect.top);
        unsigned int x_max = std::max(horiz_rect.left + horiz_rect.width - 1,
                                      vert_rect.left + vert_rect.width - 1);
        unsigned int y_max = std::max(horiz_rect.top + horiz_rect.height - 1,
                                      vert_rect.top + vert_rect.height - 1);

        set_coords(sf::IntRect(x_min, y_min,
                               x_max - x_min + 1, y_max - y_min + 1));

        // Add the surrounding walls as potential connection points.
        // This leads to some spurious invalid connection points, but that's
        // okay as they'll be discarded when checked for validity.

        // Horizontal rectangle, horizontal walls...
        for (int x_coord = horiz_rect.left + 1;
                 x_coord <= horiz_rect.left + horiz_rect.width - 1;
                 ++x_coord)
        {
          add_growth_vector(GeoVector(x_coord, horiz_rect.top - 1, Direction::North));
          add_growth_vector(GeoVector(x_coord, horiz_rect.top + horiz_rect.height, Direction::South));
        }
        // Vertical rectangle, horizontal walls...
        for (int x_coord = vert_rect.left + 1;
                 x_coord <= vert_rect.left + vert_rect.width - 1;
                 ++x_coord)
        {
          add_growth_vector(GeoVector(x_coord, vert_rect.top - 1, Direction::North));
          add_growth_vector(GeoVector(x_coord, vert_rect.top + vert_rect.height, Direction::South));
        }
        // Horizontal rectangle, vertical walls...
        for (int y_coord = horiz_rect.top + 1;
                 y_coord <= horiz_rect.top + horiz_rect.height - 1;
                 ++y_coord)
        {
          add_growth_vector(GeoVector(horiz_rect.left - 1, y_coord, Direction::West));
          add_growth_vector(GeoVector(horiz_rect.left + horiz_rect.width, y_coord, Direction::East));
        }
        // Vertical rectangle, vertical walls...
        for (int y_coord = vert_rect.top + 1;
                 y_coord <= vert_rect.top + vert_rect.height - 1;
                 ++y_coord)
        {
          add_growth_vector(GeoVector(vert_rect.left - 1, y_coord, Direction::West));
          add_growth_vector(GeoVector(vert_rect.left + vert_rect.width, y_coord, Direction::East));
        }

        /// @todo Put either a door or an open area at the starting coords.
        ///       Right now we just make it an open area.
        auto& startTile = get_map().get_tile(starting_coords.x,
                                             starting_coords.y);
        startTile->set_type(MapTileType::FloorStone);

        return true;
      }
    }

    ++num_tries;
  }

  return false;
}