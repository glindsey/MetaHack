#include "Map.h"

#include <boost/random/uniform_int_distribution.hpp>
#include <climits>
#include <memory>
#include <vector>

#include "App.h"
#include "ErrorHandler.h"
#include "Inventory.h"
#include "IsType.h"
#include "LightSource.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "ShaderEffect.h"
#include "ThingFactory.h"
#include "TileSheet.h"

#include "mapfeatures/MapFeature.h"
#include "mapfeatures/MapGenerator.h"

#define TILEID(x, y) (impl->tiles[get_index((x), (y))])
#define TILE(x, y) TF.get_tile(TILEID(x, y))

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

struct Map::Impl
{
    /// Map ID.
    MapId map_id;

    /// Map size.
    sf::Vector2i map_size;

    /// "Seen" map vertex array.
    sf::VertexArray map_seen_vertices;

    /// Outlines map vertex array.
    sf::VertexArray map_outline_vertices;

    /// "Hidden" map vertex array.
    /// @deprecated
    sf::VertexArray map_hidden_vertices;

    /// "Memory" map vertex array.
    sf::VertexArray map_memory_vertices;

    /// Thing vertex array.
    sf::VertexArray thing_vertices;

    /// Vector of tiles.
    std::vector<ThingId> tiles;

    /// Player starting location.
    sf::Vector2i start_coords;

    /// Pointer deque of map features.
    boost::ptr_deque<MapFeature> features;
};

const sf::Color Map::ambient_light_level = sf::Color(48, 48, 48, 255);

Map::Map(MapId mapId, int width, int height)
  : impl(new Impl())
{
  TRACE("Creating map of size %d x %d...", width, height);

  // Set height, width, center variables.
  impl->map_id = mapId;
  impl->map_size.x = width;
  impl->map_size.y = height;

  impl->tiles.resize(width * height);

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      ThingId tile = TF.create_tile(mapId, x, y);

      impl->tiles[get_index(x, y)] = tile;
      TILE(x, y).set_coords(x, y);
      TILE(x, y).set_ambient_light_level(ambient_light_level);
    }
  }

  // Create the vertex arrays.
  impl->map_seen_vertices.clear();
  impl->map_seen_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  impl->map_hidden_vertices.clear();
  impl->map_hidden_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  impl->map_memory_vertices.clear();
  impl->map_memory_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  impl->thing_vertices.clear();
  impl->thing_vertices.setPrimitiveType(sf::PrimitiveType::Quads);

  // Generate the map.
  if ((width != 1) && (height != 1))
  {
    MapGenerator generator(*this);
    generator.generate();
  }

  TRACE("Map created.");
}

Map::~Map()
{
  //dtor
}

int Map::get_index(int x, int y) const
{
  return (y * impl->map_size.x) + x;
}

bool Map::is_in_bounds(int x, int y) const
{
  return ((x >= 0) && (y >= 0) &&
          (x < impl->map_size.x) &&
          (y < impl->map_size.y));
}

bool Map::calc_coords(sf::Vector2i origin,
                     Direction direction,
                     sf::Vector2i& result)
{
  bool is_in_bounds = false;
  switch (direction)
  {
  case Direction::Northwest:
    if ((origin.x > 0) &&
        (origin.y > 0))
    {
      result = sf::Vector2i(origin.x - 1, origin.y - 1);
      is_in_bounds = true;
    }
    break;
  case Direction::North:
    if (origin.y > 0)
    {
      result = sf::Vector2i(origin.x, origin.y - 1);
      is_in_bounds = true;
    }
    break;
  case Direction::Northeast:
    if ((origin.x < impl->map_size.x - 1) &&
        (origin.y > 0))
    {
      result = sf::Vector2i(origin.x + 1, origin.y - 1);
      is_in_bounds = true;
    }
    break;
  case Direction::East:
    if (origin.x < impl->map_size.x - 1)
    {
      result = sf::Vector2i(origin.x + 1, origin.y);
      is_in_bounds = true;
    }
    break;
  case Direction::Southeast:
    if ((origin.x < impl->map_size.x - 1) &&
        (origin.y < impl->map_size.y - 1))
    {
      result = sf::Vector2i(origin.x + 1, origin.y + 1);
      is_in_bounds = true;
    }
    break;
  case Direction::South:
    if (origin.y < impl->map_size.y - 1)
    {
      result = sf::Vector2i(origin.x, origin.y + 1);
      is_in_bounds = true;
    }
    break;
  case Direction::Southwest:
    if ((origin.x > 0) &&
        (origin.y < impl->map_size.y - 1))
    {
      result = sf::Vector2i(origin.x - 1, origin.y + 1);
      is_in_bounds = true;
    }
    break;
  case Direction::West:
    if (origin.x < impl->map_size.x - 1)
    {
      result = sf::Vector2i(origin.x - 1, origin.y);
      is_in_bounds = true;
    }
    break;
  case Direction::Self:
  default:
    result = origin;
    is_in_bounds = true;
    break;
  }

  return is_in_bounds;
}

MapId Map::get_map_id() const
{
  return impl->map_id;
}

sf::Vector2i const& Map::get_size() const
{
  return impl->map_size;
}

sf::Vector2i const& Map::get_start_coords() const
{
  return impl->start_coords;
}

bool Map::set_start_location(sf::Vector2i startLocation)
{
  if (is_in_bounds(startLocation.x, startLocation.y))
  {
    impl->start_coords = startLocation;
    return true;
  }
  return false;
}

void Map::gather_thing_ids(std::vector<ThingId>& ids)
{
  ids.clear();
  for (int y = 0; y < impl->map_size.y; ++y)
  {
    for (int x = 0; x < impl->map_size.x; ++x)
    {
      MapTile& tile = TILE(x, y);
      tile.gather_thing_ids(ids);
    }
  }
}

void Map::update_lighting()
{
  // Clear it first.
  for (int y = 0; y < impl->map_size.y; ++y)
  {
    for (int x = 0; x < impl->map_size.x; ++x)
    {
      MapTile& tile = TILE(x, y);
      tile.clear_light_influences();
    }
  }

  for (int y = 0; y < impl->map_size.y; ++y)
  {
    for (int x = 0; x < impl->map_size.x; ++x)
    {
      MapTile& tile = TILE(x, y);
      for (ThingMapById::iterator iter = tile.get_inventory().by_id_begin();
           iter != tile.get_inventory().by_id_end();
           ++iter)
      {
        /****** GSL -- LEFT OFF HERE **********************/
        add_light((*iter).first);
      }
    }
  }
}

void Map::do_recursive_lighting(ThingId source,
                              sf::Vector2i const& origin,
                              sf::Color const& light_color,
                              int const max_depth_squared,
                              int octant,
                              int depth,
                              float slope_A,
                              float slope_B)
{
  //TRACE("origin (%d, %d), light (%d, %d, %d)", origin.x, origin.y, light_color.r, light_color.g, light_color.b);
  //TRACE("maxd2 %d, octant %d, depth %d, slope_A %1.5f, slope_B %1.5f", max_depth_squared, octant, depth, slope_A, slope_B);

  int x = 0;
  int y = 0;

  int eX = origin.x;
  int eY = origin.y;

  sf::Color addColor;

  switch (octant)
  {
    case 1:
      y = eY - depth;
      x = rintf(static_cast<float>(eX) - (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x - 1, y).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  1, depth + 1,
                                  slope_A, calc_slope(x - 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x - 1, y).is_opaque())
            {
              slope_A = calc_slope(x - 0.5, y - 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        ++x;
      }
      --x;
      break;
    case 2:
      y = eY - depth;
      x = rintf(static_cast<float>(eX) + (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x + 1, y).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  2, depth + 1,
                                  slope_A, calc_slope(x + 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x + 1, y).is_opaque())
            {
              slope_A = -calc_slope(x + 0.5, y - 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        --x;
      }
      ++x;
      break;
    case 3:
      x = eX + depth;
      y = rintf(static_cast<float>(eY) - (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x, y - 1).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  3, depth + 1,
                                  slope_A, calc_inv_slope(x - 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x, y - 1).is_opaque())
            {
              slope_A = -calc_inv_slope(x + 0.5, y - 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        ++y;
      }
      --y;
      break;
    case 4:
      x = eX + depth;
      y = rintf(static_cast<float>(eY) + (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x, y + 1).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  4, depth + 1,
                                  slope_A, calc_inv_slope(x - 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x, y + 1).is_opaque())
            {
              slope_A = calc_inv_slope(x + 0.5, y + 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        --y;
      }
      ++y;
      break;
    case 5:
      y = eY + depth;
      x = rintf(static_cast<float>(eX) + (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x + 1, y).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  5, depth + 1,
                                  slope_A, calc_slope(x + 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x + 1, y).is_opaque())
            {
              slope_A = calc_slope(x + 0.5, y + 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        --x;
      }
      ++x;
      break;
    case 6:
      y = eY + depth;
      x = rintf(static_cast<float>(eX) - (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x - 1, y).is_opaque())
            {
              do_recursive_lighting(source,origin, light_color,
                                  max_depth_squared,
                                  6, depth + 1,
                                  slope_A, calc_slope(x - 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x - 1, y).is_opaque())
            {
              slope_A = -calc_slope(x - 0.5, y + 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        ++x;
      }
      --x;
      break;
    case 7:
      x = eX - depth;
      y = rintf(static_cast<float>(eY) + (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x, y + 1).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  7, depth + 1,
                                  slope_A, calc_inv_slope(x + 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x, y + 1).is_opaque())
            {
              slope_A = -calc_inv_slope(x - 0.5, y + 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        --y;
      }
      ++y;
      break;
    case 8:
      x = eX - depth;
      y = rintf(static_cast<float>(eY) - (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= max_depth_squared)
        {
          if (get_tile(x, y).is_opaque())
          {
            if (!get_tile(x, y - 1).is_opaque())
            {
              do_recursive_lighting(source, origin, light_color,
                                  max_depth_squared,
                                  8, depth + 1,
                                  slope_A, calc_inv_slope(x + 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (get_tile(x, y - 1).is_opaque())
            {
              slope_A = calc_inv_slope(x - 0.5, y - 0.5, eX, eY);
            }
          }

          MapTile::LightInfluence influence;
          influence.coords = origin;
          influence.color = light_color;
          influence.intensity = max_depth_squared;
          get_tile(x, y).add_light_influence(source, influence);

        }
        ++y;
      }
      --y;
      break;
    default:
      MAJOR_ERROR("Octant passed to do_recursive_lighting was %d (not 1 to 8)!", octant);
      break;
  }

  if ((depth*depth < max_depth_squared) && (!get_tile(x, y).is_opaque()))
  {
    do_recursive_lighting(source, origin, light_color,
                        max_depth_squared,
                        octant, depth + 1,
                        slope_A, slope_B);
  }
}

void Map::add_light(ThingId thing_id)
{
  Thing& thing = TF.get(thing_id);

  // First make sure the thing is a light source.
  if (isType(&thing, LightSource))
  {
    LightSource& source = dynamic_cast<LightSource&>(thing);

    // Get the thing's absolute location.
    ThingId thing_location = thing.get_root_id();
    Container& location = TF.get_container(thing_location);

    if (!location.is_maptile() || (location.get_map_id() != impl->map_id))
    {
      // Return, as the light source is not on the map proper.
      return;
    }

    sf::Vector2i coords = TF.get_tile(thing_location).get_coords();

    sf::Color light_color = source.get_light_color();
    Direction light_direction = source.get_light_direction();

    /// @todo Handle the special case of Direction::Self.
    /// If a light source's direction is set to "Self", it should be treated as
    /// omnidirectional but dimmer when not held by an Entity, and the same
    /// direction as the Entity when it is held.

    /// @todo: Handle "dark sources" with negative light strength properly --
    ///        right now they'll cause Very Bad Behavior!
    int max_depth_squared = source.get_light_strength();

    // Add a light influence to the tile the light is on.
    MapTile::LightInfluence influence;
    influence.coords = coords;
    influence.color = light_color;
    influence.intensity = max_depth_squared;
    get_tile(coords.x, coords.y).add_light_influence(thing_id, influence);

    // Octant is an integer representing the following:
    // \ 1|2 /  |
    //  \ | /   |
    // 8 \|/ 3  |
    // ---+---  |
    // 7 /|\ 4  |
    //  / | \   |
    // / 6|5 \  |

    // Directional lighting:
    // Direction  1 2 3 4 5 6 7 8
    // ==========================
    // Self/Up/Dn x x x x x x x x
    // North      x x - - - - - -
    // Northeast  - x x - - - - -
    // East       - - x x - - - -
    // Southeast  - - - x x - - -
    // South      - - - - x x - -
    // Southwest  - - - - - x x -
    // West       - - - - - - x x
    // Northwest  x - - - - - - x

    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::Northwest) ||
        (light_direction == Direction::North))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 1);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::North) ||
        (light_direction == Direction::Northeast))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 2);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::Northeast) ||
        (light_direction == Direction::East))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 3);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::East) ||
        (light_direction == Direction::Southeast))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 4);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::Southeast) ||
        (light_direction == Direction::South))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 5);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::South) ||
        (light_direction == Direction::Southwest))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 6);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::Southwest) ||
        (light_direction == Direction::West))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 7);
    }
    if ((light_direction == Direction::Self) ||
        (light_direction == Direction::Up) ||
        (light_direction == Direction::Down) ||
        (light_direction == Direction::West) ||
        (light_direction == Direction::Northwest))
    {
      do_recursive_lighting(thing_id, coords, light_color, max_depth_squared, 8);
    }
  }

  if (thing.is_container())
  {
    /// @todo Check container's opacity; otherwise we get stuff shining right
    ///       through boxes and bags!
    Container& container = static_cast<Container&>(thing);

    for (ThingMapById::iterator iter = container.get_inventory().by_id_begin();
         iter != container.get_inventory().by_id_end(); ++iter)
    {
      add_light((*iter).first);
    }
  }
}

void Map::update_tile_vertices(Entity& entity)
{
  ThingId entity_location_id = entity.get_location_id();
  Container& location = TF.get_container(entity_location_id);

  if (!location.is_maptile() || (location.get_map_id() != impl->map_id))
  {
    // Error, as the entity isn't actually on this map!
    MAJOR_ERROR("Map::update_tile_vertices called for an entity which isn't on that map!");
    return;
  }

  static sf::Vector2f position;

  // Loop through and draw tiles.
  impl->map_seen_vertices.clear();
  impl->map_hidden_vertices.clear();
  impl->map_memory_vertices.clear();

  for (int y = 0; y < impl->map_size.y; ++y)
  {
    for (int x = 0; x < impl->map_size.x; ++x)
    {
      MapTile& tile = TILE(x, y);

      if (entity.can_see(x, y))
      {
        tile.add_vertices_to(impl->map_seen_vertices, true);
      }
      else
      {
        //tile.add_vertices_to(impl->map_hidden_vertices, false);
        entity.add_memory_vertices_to(impl->map_memory_vertices, x, y);
      }
    } // end for (int x)
  } // end for (int y)

  // Do walls.
  for (int y = 0; y < impl->map_size.y; ++y)
  {
    for (int x = 0; x < impl->map_size.x; ++x)
    {
      MapTile& tile = TILE(x, y);

      bool this_is_empty = tile.is_empty_space();
      if (!this_is_empty)
      {
        bool nw_is_empty = ((x > 0) && (y > 0))
                            ? (entity.can_see(x-1, y-1) &&
                               TILE(x-1, y-1).is_empty_space()) : false;
        bool n_is_empty  = (y > 0)
                            ? (entity.can_see(x, y-1) &&
                               TILE(x, y-1).is_empty_space()) : false;
        bool ne_is_empty = ((x < impl->map_size.x - 1) && (y > 0))
                            ? (entity.can_see(x+1, y-1) &&
                               TILE(x+1, y-1).is_empty_space()) : false;
        bool e_is_empty  = (x < impl->map_size.x - 1)
                            ? (entity.can_see(x+1, y) &&
                               TILE(x+1, y).is_empty_space()) : false;
        bool se_is_empty = ((x < impl->map_size.x - 1) &&
                            (y < impl->map_size.y - 1))
                            ? (entity.can_see(x+1, y+1) &&
                               TILE(x+1, y+1).is_empty_space()) : false;
        bool s_is_empty  = (y < impl->map_size.y - 1)
                            ? (entity.can_see(x, y+1) &&
                               TILE(x, y+1).is_empty_space()) : false;
        bool sw_is_empty = ((x > 0) && (y < impl->map_size.y - 1))
                            ? (entity.can_see(x-1, y+1) &&
                               TILE(x-1, y+1).is_empty_space()) : false;
        bool w_is_empty  = (x > 0)
                            ? (entity.can_see(x-1, y) &&
                               TILE(x-1, y).is_empty_space()) : false;

        if (entity.can_see(x, y))
        {
          tile.add_walls_to(impl->map_seen_vertices, true,
                            nw_is_empty, n_is_empty,
                            ne_is_empty, e_is_empty,
                            se_is_empty, s_is_empty,
                            sw_is_empty, w_is_empty);
        }
        else
        {
          //tile.add_walls_to(impl->map_hidden_vertices, false,
          //                  nw_is_empty, n_is_empty,
          //                  ne_is_empty, e_is_empty,
          //                  se_is_empty, s_is_empty,
          //                  sw_is_empty, w_is_empty);

          //entity.add_memory_walls_to(impl->map_memory_vertices, x, y);
        }
      } // end if (this_is_empty)
    } // end for (int x)
  } // end for (int y)

}

void Map::update_thing_vertices(Entity& entity, int frame)
{
  ThingId entity_location_id = entity.get_location_id();
  ThingId entity_id = entity.get_id();
  Container& location = TF.get_container(entity_location_id);

  if (!location.is_maptile() || (location.get_map_id() != impl->map_id))
  {
    // Error, as the entity isn't actually on this map!
    MAJOR_ERROR("Map::update_thing_vertices called for entity that isn't on that map!");
    return;
  }

  static sf::Vector2f position;

  // Loop through and draw things.
  impl->thing_vertices.clear();
  for (int y = 0; y < impl->map_size.y; ++y)
  {
    for (int x = 0; x < impl->map_size.x; ++x)
    {
      MapTile& tile = TILE(x, y);
      Inventory& inv = tile.get_inventory();

      if (entity.can_see(x, y))
      {
        if (inv.count() > 0)
        {
          Thing& biggest_thing = TF.get(inv.get_largest_thing_id());
          biggest_thing.add_vertices_to(impl->thing_vertices, true, frame);
        }
      }

      if (inv.contains(entity_id))
      {
        entity.add_vertices_to(impl->thing_vertices, true, frame);
      }
    }
  }
}

void Map::set_view(sf::RenderTarget& target,
                   sf::Vector2f center,
                   float zoom_level)
{
  sf::Vector2u screen_size = target.getSize();

  sf::Vector2f window_center = sf::Vector2f((screen_size.x / zoom_level) / 2,
                                            (screen_size.y / zoom_level) / 2);

  target.setView(sf::View(sf::FloatRect((center.x - window_center.x),
                                        (center.y - window_center.y),
                                        (screen_size.x / zoom_level),
                                        (screen_size.y / zoom_level))));
}

void Map::draw_to(sf::RenderTarget& target)
{
  the_shader.setParameter("texture", sf::Shader::CurrentTexture);

  sf::RenderStates render_states = sf::RenderStates::Default;
  render_states.shader = &the_shader;
  render_states.texture = &(the_tile_sheet.getTexture());

  the_shader.setParameter("effect", ShaderEffect::Lighting);
  target.draw(impl->map_seen_vertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Sepia);
  target.draw(impl->map_memory_vertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Lighting);
  target.draw(impl->thing_vertices, render_states);
}

ThingId Map::get_tile_id(int x, int y) const
{
  if (x < 0) x = 0;
  if (x >= impl->map_size.x) x = impl->map_size.x - 1;
  if (y < 0) y = 0;
  if (y >= impl->map_size.y) y = impl->map_size.y - 1;

  return TILEID(x, y);
}

MapTile& Map::get_tile(int x, int y) const
{
  if (x < 0) x = 0;
  if (x >= impl->map_size.x) x = impl->map_size.x - 1;
  if (y < 0) y = 0;
  if (y >= impl->map_size.y) y = impl->map_size.y - 1;

  return TILE(x, y);
}

MapTile& Map::get_tile(sf::Vector2i tile) const
{
  return get_tile(tile.x, tile.y);
}

void Map::clear_map_features()
{
  impl->features.clear();
}

MapFeature& Map::get_random_map_feature()
{
  uniform_int_dist featureDist(0, impl->features.size() - 1);
  int featureIndex = featureDist(the_RNG);
  return impl->features[featureIndex];
}

boost::ptr_deque<MapFeature> const& Map::get_map_features() const
{
  return impl->features;
}

MapFeature& Map::add_map_feature(MapFeature* feature)
{
  impl->features.push_back(feature);
  return *feature;
}
