#include "Map.h"
#include <boost/log/trivial.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <climits>
#include <memory>

#include "App.h"
#include "ConfigSettings.h"
#include "GameState.h"
#include "Inventory.h"
#include "LightInfluence.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "ShaderEffect.h"
#include "ThingManager.h"
#include "TileSheet.h"

#include "MapFeature.h"
#include "MapGenerator.h"

#define VERTEX(x, y) (20 * (pImpl->map_size.x * y) + x)
#define TILE(x, y) (pImpl->tiles[get_index(x, y)])

// Local typedefs
typedef boost::random::uniform_int_distribution<> uniform_int_dist;

const sf::Color Map::ambient_light_level = sf::Color(48, 48, 48, 255);

Map::Map(MapId map_id, int width, int height)
  :
  pImpl(map_id, width, height),
  m_generator{ NEW MapGenerator(*this) }
{
  TRACE("Creating map of size %d x %d...", width, height);

  // Create vertices:
  // 4 vertices * 4 quads for the floor
  // 4 vertices * 4 quads * 4 potential walls
  // = 16 + 64 = 80 possible vertices per tile.
  pImpl->map_seen_vertices.resize(pImpl->map_size.x * pImpl->map_size.y * 80);
  pImpl->map_memory_vertices.resize(pImpl->map_size.x * pImpl->map_size.y * 80);

  // Create the tiles themselves.
  MetadataCollection& collection = MDC::get_collection("maptile");
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      Metadata& metadata = collection.get("MTUnknown");

      MapTile* new_tile = NEW MapTile({ x, y }, metadata, map_id);
      pImpl->tiles.push_back(new_tile);
      new_tile->set_coords(x, y);
      new_tile->set_ambient_light_level(ambient_light_level);
    }
  }

  // Create the vertex arrays.
  pImpl->map_seen_vertices.clear();
  pImpl->map_seen_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  pImpl->map_memory_vertices.clear();
  pImpl->map_memory_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
  pImpl->thing_vertices.clear();
  pImpl->thing_vertices.setPrimitiveType(sf::PrimitiveType::Quads);

  TRACE("Map created.");
}

void Map::initialize()
{
  // If the map isn't the 1x1 "limbo" map...
  if ((pImpl->map_size.x != 1) && (pImpl->map_size.y != 1))
  {
    // Generate the map.
    m_generator->generate();

    // Run Lua script associated with this map.
    /// @todo Different scripts for different maps.
    ///       And for that matter, ALL of map generation
    ///       should be done via scripting. But for now
    ///       this will do.
    TRACE("Executing Map Lua script.");
    the_lua_instance.set_global("current_map_id", pImpl->map_id);
    the_lua_instance.require("resources/scripts/map");
  }

  TRACE("Map initialized.");
}

Map::~Map()
{
  //dtor
}

int Map::get_index(int x, int y) const
{
  return (y * pImpl->map_size.x) + x;
}

bool Map::is_in_bounds(int x, int y) const
{
  return ((x >= 0) && (y >= 0) &&
          (x < pImpl->map_size.x) &&
          (y < pImpl->map_size.y));
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
      if ((origin.x < pImpl->map_size.x - 1) &&
          (origin.y > 0))
      {
        result = sf::Vector2i(origin.x + 1, origin.y - 1);
        is_in_bounds = true;
      }
      break;
    case Direction::East:
      if (origin.x < pImpl->map_size.x - 1)
      {
        result = sf::Vector2i(origin.x + 1, origin.y);
        is_in_bounds = true;
      }
      break;
    case Direction::Southeast:
      if ((origin.x < pImpl->map_size.x - 1) &&
          (origin.y < pImpl->map_size.y - 1))
      {
        result = sf::Vector2i(origin.x + 1, origin.y + 1);
        is_in_bounds = true;
      }
      break;
    case Direction::South:
      if (origin.y < pImpl->map_size.y - 1)
      {
        result = sf::Vector2i(origin.x, origin.y + 1);
        is_in_bounds = true;
      }
      break;
    case Direction::Southwest:
      if ((origin.x > 0) &&
          (origin.y < pImpl->map_size.y - 1))
      {
        result = sf::Vector2i(origin.x - 1, origin.y + 1);
        is_in_bounds = true;
      }
      break;
    case Direction::West:
      if (origin.x < pImpl->map_size.x - 1)
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
  return pImpl->map_id;
}

sf::Vector2i const& Map::get_size() const
{
  return pImpl->map_size;
}

sf::Vector2i const& Map::get_start_coords() const
{
  return pImpl->start_coords;
}

bool Map::set_start_coords(sf::Vector2i start_coords)
{
  if (is_in_bounds(start_coords.x, start_coords.y))
  {
    pImpl->start_coords = start_coords;
    return true;
  }
  return false;
}

void Map::process()
{
  for (int y = 0; y < pImpl->map_size.y; ++y)
  {
    for (int x = 0; x < pImpl->map_size.x; ++x)
    {
      ThingRef contents = TILE(x, y).get_tile_contents();
      contents->process();
    }
  }
}

void Map::update_lighting()
{
  // Clear it first.
  for (int y = 0; y < pImpl->map_size.y; ++y)
  {
    for (int x = 0; x < pImpl->map_size.x; ++x)
    {
      TILE(x, y).clear_light_influences();
    }
  }

  for (int y = 0; y < pImpl->map_size.y; ++y)
  {
    for (int x = 0; x < pImpl->map_size.x; ++x)
    {
      ThingRef contents = TILE(x, y).get_tile_contents();
      auto& inventory = contents->get_inventory();
      auto& things = inventory.get_things();
      for (auto iter = std::begin(things);
      iter != std::end(things);
        ++iter)
      {
        ThingRef thing = iter->second;
        thing->light_up_surroundings();
        //add_light(thing);
      }
    }
  }
}

void Map::do_recursive_lighting(ThingRef source,
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

  sf::Vector2i new_coords;

  sf::Color addColor;

  std::function< bool(sf::Vector2f, sf::Vector2f, float) > loop_condition;
  Direction dir;
  std::function< float(sf::Vector2f, sf::Vector2f) > recurse_slope;
  std::function< float(sf::Vector2f, sf::Vector2f) > loop_slope;

  switch (octant)
  {
    case 1:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y - depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + halfunit(Direction::Southwest), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + halfunit(Direction::Northwest), b); };
      break;

    case 2:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y - depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + halfunit(Direction::Southeast), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_slope(a + halfunit(Direction::Northeast), b); };
      break;

    case 3:
      new_coords.x = origin.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + halfunit(Direction::Northwest), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_inv_slope(a + halfunit(Direction::Northeast), b); };
      break;

    case 4:
      new_coords.x = origin.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + halfunit(Direction::Southwest), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + halfunit(Direction::Southeast), b); };
      break;

    case 5:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y + depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + halfunit(Direction::Northeast), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + halfunit(Direction::Southeast), b); };
      break;

    case 6:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y + depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + halfunit(Direction::Northwest), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_slope(a + halfunit(Direction::Southwest), b); };
      break;

    case 7:
      new_coords.x = origin.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + halfunit(Direction::Southeast), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_inv_slope(a + halfunit(Direction::Southwest), b); };
      break;

    case 8:
      new_coords.x = origin.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + halfunit(Direction::Northeast), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + halfunit(Direction::Northwest), b); };
      break;

    default:
      MAJOR_ERROR("Octant passed to do_recursive_lighting was %d (not 1 to 8)!", octant);
      break;
  }

  while (loop_condition(to_v2f(new_coords), to_v2f(origin), slope_B))
  {
    if (calc_vis_distance(new_coords, origin) <= max_depth_squared)
    {
      if (get_tile(new_coords).is_opaque())
      {
        if (!get_tile(new_coords + unit(dir)).is_opaque())
        {
          do_recursive_lighting(source, origin, light_color,
                                max_depth_squared,
                                octant, depth + 1,
                                slope_A, recurse_slope(to_v2f(new_coords), to_v2f(origin)));
        }
      }
      else
      {
        if (get_tile(new_coords + unit(dir)).is_opaque())
        {
          slope_A = loop_slope(to_v2f(new_coords), to_v2f(origin));
        }
      }

      LightInfluence influence;
      influence.coords = origin;
      influence.color = light_color;
      influence.intensity = max_depth_squared;
      get_tile(new_coords).add_light_influence(source, influence);
    }
    new_coords -= unit(dir);
  }
  new_coords += unit(dir);

  if ((depth*depth < max_depth_squared) && (!get_tile(new_coords).is_opaque()))
  {
    do_recursive_lighting(source, origin, light_color,
                          max_depth_squared,
                          octant, depth + 1,
                          slope_A, slope_B);
  }
}

void Map::add_light(ThingRef source)
{
  // Get the map tile the light source is on.
  auto maptile = source->get_maptile();
  if (maptile == nullptr)
  {
    return;
  }

  /// @todo Check if any opaque containers are between the light source and the map.

  sf::Vector2i coords = maptile->get_coords();

  int light_color_red = source->get_property<int>("light_color_red");
  int light_color_green = source->get_property<int>("light_color_green");
  int light_color_blue = source->get_property<int>("light_color_blue");
  sf::Color light_color = sf::Color(light_color_red, light_color_green, light_color_blue, 255);

  /// @todo Re-implement direction.
  Direction light_direction = Direction::Up;

  /// @todo Handle the special case of Direction::Self.
  /// If a light source's direction is set to "Self", it should be treated as
  /// omnidirectional but dimmer when not held by an Entity, and the same
  /// direction as the Entity when it is held.

  /// @todo: Handle "dark sources" with negative light strength properly --
  ///        right now they'll cause Very Bad Behavior!
  int max_depth_squared = source->get_property<int>("light_strength");

  // Add a light influence to the tile the light is on.
  LightInfluence influence;
  influence.coords = coords;
  influence.color = light_color;
  influence.intensity = max_depth_squared;
  get_tile(coords.x, coords.y).add_light_influence(source, influence);

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
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 1);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::North) ||
      (light_direction == Direction::Northeast))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 2);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Northeast) ||
      (light_direction == Direction::East))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 3);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::East) ||
      (light_direction == Direction::Southeast))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 4);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Southeast) ||
      (light_direction == Direction::South))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 5);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::South) ||
      (light_direction == Direction::Southwest))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 6);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::Southwest) ||
      (light_direction == Direction::West))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 7);
  }
  if ((light_direction == Direction::Self) ||
      (light_direction == Direction::Up) ||
      (light_direction == Direction::Down) ||
      (light_direction == Direction::West) ||
      (light_direction == Direction::Northwest))
  {
    do_recursive_lighting(source, coords, light_color, max_depth_squared, 8);
  }
}

void Map::update_tile_vertices(ThingRef thing)
{
  auto location = thing->get_location();

  static sf::Vector2f position;

  // Loop through and draw tiles.
  pImpl->map_seen_vertices.clear();
  pImpl->map_memory_vertices.clear();

  for (int y = 0; y < pImpl->map_size.y; ++y)
  {
    for (int x = 0; x < pImpl->map_size.x; ++x)
    {
      auto& tile = TILE(x, y);
      bool this_is_empty = tile.is_empty_space();
      bool nw_is_empty = ((x > 0) && (y > 0)) ? (thing->can_see(x - 1, y - 1) && TILE(x - 1, y - 1).is_empty_space()) : false;
      bool n_is_empty = (y > 0) ? (thing->can_see(x, y - 1) && TILE(x, y - 1).is_empty_space()) : false;
      bool ne_is_empty = ((x < pImpl->map_size.x - 1) && (y > 0)) ? (thing->can_see(x + 1, y - 1) && TILE(x + 1, y - 1).is_empty_space()) : false;
      bool e_is_empty = (x < pImpl->map_size.x - 1) ? (thing->can_see(x + 1, y) && TILE(x + 1, y).is_empty_space()) : false;
      bool se_is_empty = ((x < pImpl->map_size.x - 1) && (y < pImpl->map_size.y - 1)) ? (thing->can_see(x + 1, y + 1) && TILE(x + 1, y + 1).is_empty_space()) : false;
      bool s_is_empty = (y < pImpl->map_size.y - 1) ? (thing->can_see(x, y + 1) && TILE(x, y + 1).is_empty_space()) : false;
      bool sw_is_empty = ((x > 0) && (y < pImpl->map_size.y - 1)) ? (thing->can_see(x - 1, y + 1) && TILE(x - 1, y + 1).is_empty_space()) : false;
      bool w_is_empty = (x > 0) ? (thing->can_see(x - 1, y) && TILE(x - 1, y).is_empty_space()) : false;

      if (thing->can_see(x, y))
      {
        if (this_is_empty)
        {
          tile.add_floor_vertices_to(pImpl->map_seen_vertices, true);
        }
        else
        {
          tile.add_wall_vertices_to(pImpl->map_seen_vertices, true,
                                    nw_is_empty, n_is_empty,
                                    ne_is_empty, e_is_empty,
                                    se_is_empty, s_is_empty,
                                    sw_is_empty, w_is_empty);
        }
      }
      else
      {
        thing->add_memory_vertices_to(pImpl->map_memory_vertices, x, y);
      }
    } // end for (int x)
  } // end for (int y)
}

void Map::update_thing_vertices(ThingRef thing, int frame)
{
  // Loop through and draw things.
  pImpl->thing_vertices.clear();
  for (int y = 0; y < pImpl->map_size.y; ++y)
  {
    for (int x = 0; x < pImpl->map_size.x; ++x)
    {
      ThingRef contents = TILE(x, y).get_tile_contents();
      Inventory& inv = contents->get_inventory();

      if (thing->can_see(x, y))
      {
        if (inv.count() > 0)
        {
          ThingRef biggest_thing = inv.get_largest_thing();
          if (biggest_thing != ThingManager::get_mu())
          {
            biggest_thing->add_floor_vertices_to(pImpl->thing_vertices, true, frame);
          }
        }
      }

      if (inv.contains(thing))
      {
        thing->add_floor_vertices_to(pImpl->thing_vertices, true, frame);
      }
    }
  }
}

void Map::set_view(sf::RenderTarget& target,
                   sf::Vector2f center,
                   float zoom_level)
{
  sf::Vector2u screen_size = target.getSize();
  unsigned int inventory_area_width = Settings.get<unsigned int>("inventory_area_width");
  unsigned int status_area_height = Settings.get<unsigned int>("status_area_height");
  unsigned int messagelog_area_height = Settings.get<unsigned int>("messagelog_area_height");

  sf::Vector2f window_center = sf::Vector2f((static_cast<float>(screen_size.x - inventory_area_width) / zoom_level) / 2,
                                            messagelog_area_height + (static_cast<float>(screen_size.y - (status_area_height + messagelog_area_height)) / zoom_level) / 2);

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
  render_states.texture = &(TS.getTexture());

  the_shader.setParameter("effect", ShaderEffect::Lighting);
  //the_shader.setParameter("effect", ShaderEffect::Default);
  target.draw(pImpl->map_seen_vertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Sepia);
  target.draw(pImpl->map_memory_vertices, render_states);
  the_shader.setParameter("effect", ShaderEffect::Lighting);
  //the_shader.setParameter("effect", ShaderEffect::Default);
  target.draw(pImpl->thing_vertices, render_states);
}

MapTile const& Map::get_tile(int x, int y) const
{
  if (x < 0) x = 0;
  if (x >= pImpl->map_size.x) x = pImpl->map_size.x - 1;
  if (y < 0) y = 0;
  if (y >= pImpl->map_size.y) y = pImpl->map_size.y - 1;

  return TILE(x, y);
}

MapTile& Map::get_tile(int x, int y)
{
  if (x < 0) x = 0;
  if (x >= pImpl->map_size.x) x = pImpl->map_size.x - 1;
  if (y < 0) y = 0;
  if (y >= pImpl->map_size.y) y = pImpl->map_size.y - 1;

  return TILE(x, y);
}

MapTile& Map::get_tile(sf::Vector2i tile)
{
  return get_tile(tile.x, tile.y);
}

bool Map::tile_is_opaque(sf::Vector2i tile)
{
  if ((tile.x < 0) || (tile.x >= pImpl->map_size.x) || (tile.y < 0) || (tile.y >= pImpl->map_size.y))
  {
    return true;
  }

  return TILE(tile.x, tile.y).is_opaque();
}

void Map::clear_map_features()
{
  pImpl->features.clear();
}

MapFeature& Map::get_random_map_feature()
{
  if (pImpl->features.size() < 1)
  {
    FATAL_ERROR("get_random_map_feature() called but map doesn't contain any features yet!");
  }

  uniform_int_dist featureDist(0, pImpl->features.size() - 1);
  int featureIndex = featureDist(the_RNG);
  return pImpl->features[featureIndex];
}

boost::ptr_deque<MapFeature> const& Map::get_map_features() const
{
  return pImpl->features;
}

MapFeature& Map::add_map_feature(MapFeature* feature)
{
  if (feature != nullptr)
  {
    pImpl->features.push_back(feature);
  }
  return *feature;
}

int Map::LUA_get_tile_contents(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 3)
  {
    MINOR_ERROR("expected 3 arguments, got %d", num_args);
    return 0;
  }

  MapId map_id = static_cast<MapId>(static_cast<unsigned int>(lua_tointeger(L, 1)));
  sf::Vector2i coords = sf::Vector2i(static_cast<int>(lua_tointeger(L, 2)), static_cast<int>(lua_tointeger(L, 3)));

  auto& map_tile = GAME.get_map_factory().get(map_id).get_tile(coords);
  ThingRef contents = map_tile.get_tile_contents();

  lua_pushinteger(L, contents);

  return 1;
}

int Map::LUA_get_start_coords(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    MINOR_ERROR("expected 1 arguments, got %d", num_args);
    return 0;
  }

  MapId map_id = static_cast<MapId>(static_cast<unsigned int>(lua_tointeger(L, 1)));

  auto& map = GAME.get_map_factory().get(map_id);
  auto coords = map.get_start_coords();

  lua_pushinteger(L, coords.x);
  lua_pushinteger(L, coords.y);

  return 2;
}

int Map::LUA_map_add_feature(lua_State* L)
{
  PropertyDictionary feature_settings;

  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    MINOR_ERROR("expected 2 arguments, got %d", num_args);
    return 0;
  }

  MapId map_id = static_cast<MapId>(static_cast<unsigned int>(lua_tointeger(L, 1)));

  auto& map = GAME.get_map_factory().get(map_id);

  std::string feature = lua_tostring(L, 2);

  feature_settings.set<std::string>("type", feature);

  bool success = map.m_generator->add_feature(feature_settings);

  lua_pushboolean(L, static_cast<int>(success));

  return 1;
}