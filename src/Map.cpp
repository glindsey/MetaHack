#include "stdafx.h"

#include "Map.h"

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

struct Map::Impl
{
  /// Pointer vector of tiles.
  boost::ptr_vector< MapTile > tiles;

  /// Player starting location.
  Vec2i start_coords;

  /// Pointer deque of map features.
  boost::ptr_deque<MapFeature> features;
};

/// @todo Have this take an Vec2i instead of width x height
Map::Map(GameState& game, MapId map_id, int width, int height)
  :
  m_game{ game },
  m_map_id{ map_id },
  m_map_size{ width, height },
  m_generator{ NEW MapGenerator(*this) },
  pImpl{ NEW Impl{} }
{
  SET_UP_LOGGER("Map", true);

  CLOG(TRACE, "Map") << "Creating map of size " << width << " x " << height;

  // Create the tiles themselves.
  MetadataCollection& collection = m_game.get_metadata_collection("maptile");
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

  CLOG(TRACE, "Map") << "Map created.";
}

void Map::initialize()
{
  // If the map isn't the 1x1 "limbo" map...
  if ((m_map_size.x != 1) && (m_map_size.y != 1))
  {
    // Generate the map.
    m_generator->generate();

    // Run Lua script associated with this map.
    /// @todo Different scripts for different maps.
    ///       And for that matter, ALL of map generation
    ///       should be done via scripting. But for now
    ///       this will do.
    CLOG(TRACE, "Map") << "Executing Map Lua script.";
    the_lua_instance.set_global("current_map_id", m_map_id);
    the_lua_instance.require("resources/scripts/map");
  }

  CLOG(TRACE, "Map") << "Map initialized.";
}

Map::~Map()
{
  //dtor
}

int Map::get_index(int x, int y) const
{
  return (y * m_map_size.x) + x;
}

bool Map::is_in_bounds(int x, int y) const
{
  return ((x >= 0) && (y >= 0) &&
    (x < m_map_size.x) &&
          (y < m_map_size.y));
}

bool Map::calc_coords(Vec2i origin,
                      Direction direction,
                      Vec2i& result)
{
  result = origin + (Vec2i)direction;

  bool is_in_bounds = ((result.x >= 0) &&
    (result.y >= 0) &&
                       (result.x <= m_map_size.x - 1) &&
                       (result.y <= m_map_size.y - 1));

  return is_in_bounds;
}

MapId Map::get_map_id() const
{
  return m_map_id;
}

Vec2i const& Map::get_size() const
{
  return m_map_size;
}

Vec2i const& Map::get_start_coords() const
{
  return pImpl->start_coords;
}

bool Map::set_start_coords(Vec2i start_coords)
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
  for (int y = 0; y < m_map_size.y; ++y)
  {
    for (int x = 0; x < m_map_size.x; ++x)
    {
      ThingId contents = TILE(x, y).get_tile_contents();
      contents->process();
    }
  }
}

void Map::update_lighting()
{
  // Clear it first.
  for (int y = 0; y < m_map_size.y; ++y)
  {
    for (int x = 0; x < m_map_size.x; ++x)
    {
      TILE(x, y).clear_light_influences();
    }
  }

  for (int y = 0; y < m_map_size.y; ++y)
  {
    for (int x = 0; x < m_map_size.x; ++x)
    {
      ThingId contents = TILE(x, y).get_tile_contents();
      auto& inventory = contents->get_inventory();
      for (auto iter = inventory.begin();
           iter != inventory.end();
           ++iter)
      {
        ThingId thing = iter->second;
        thing->light_up_surroundings();
        //add_light(thing);
      }
    }
  }
}

void Map::do_recursive_lighting(ThingId source,
                                Vec2i const& origin,
                                sf::Color const& light_color,
                                int const max_depth_squared,
                                int octant,
                                int depth,
                                float slope_A,
                                float slope_B)
{
  Vec2i new_coords;

  sf::Color addColor;

  std::function< bool(Vec2f, Vec2f, float) > loop_condition;
  Direction dir;
  std::function< float(Vec2f, Vec2f) > recurse_slope;
  std::function< float(Vec2f, Vec2f) > loop_slope;

  switch (octant)
  {
    case 1:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y - depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Northwest.half(), b); };
      break;

    case 2:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y - depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_slope(a + Direction::Northeast.half(), b); };
      break;

    case 3:
      new_coords.x = origin.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
      break;

    case 4:
      new_coords.x = origin.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      break;

    case 5:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y + depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Southeast.half(), b); };
      break;

    case 6:
      new_coords.x = static_cast<int>(rint(static_cast<float>(origin.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = origin.y + depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_slope(a + Direction::Southwest.half(), b); };
      break;

    case 7:
      new_coords.x = origin.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
      break;

    case 8:
      new_coords.x = origin.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(origin.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
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
        if (!get_tile(new_coords + (Vec2i)dir).is_opaque())
        {
          do_recursive_lighting(source, origin, light_color,
                                max_depth_squared,
                                octant, depth + 1,
                                slope_A, recurse_slope(to_v2f(new_coords), to_v2f(origin)));
        }
      }
      else
      {
        if (get_tile(new_coords + (Vec2i)dir).is_opaque())
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
    new_coords -= (Vec2i)dir;
  }
  new_coords += (Vec2i)dir;

  if ((depth*depth < max_depth_squared) && (!get_tile(new_coords).is_opaque()))
  {
    do_recursive_lighting(source, origin, light_color,
                          max_depth_squared,
                          octant, depth + 1,
                          slope_A, slope_B);
  }
}

void Map::add_light(ThingId source)
{
  // Get the map tile the light source is on.
  auto maptile = source->get_maptile();
  if (maptile == nullptr)
  {
    return;
  }

  /// @todo Check if any opaque containers are between the light source and the map.

  Vec2i coords = maptile->get_coords();

  int light_color_red = source->get_modified_property<int>("light_color_red");
  int light_color_green = source->get_modified_property<int>("light_color_green");
  int light_color_blue = source->get_modified_property<int>("light_color_blue");
  sf::Color light_color = sf::Color(light_color_red, light_color_green, light_color_blue, 255);
  int max_depth_squared = source->get_modified_property<int>("light_strength");

  /// @todo Re-implement direction.
  Direction light_direction = Direction::Up;

  /// @todo Handle the special case of Direction::Self.
  /// If a light source's direction is set to "Self", it should be treated as
  /// omnidirectional but dimmer when not held by an Entity, and the same
  /// direction as the Entity when it is held.

  /// @todo: Handle "dark sources" with negative light strength properly --
  ///        right now they'll cause Very Bad Behavior!

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

MapTile const& Map::get_tile(int x, int y) const
{
  if (x < 0) x = 0;
  if (x >= m_map_size.x) x = m_map_size.x - 1;
  if (y < 0) y = 0;
  if (y >= m_map_size.y) y = m_map_size.y - 1;

  return TILE(x, y);
}

MapTile& Map::get_tile(int x, int y)
{
  if (x < 0) x = 0;
  if (x >= m_map_size.x) x = m_map_size.x - 1;
  if (y < 0) y = 0;
  if (y >= m_map_size.y) y = m_map_size.y - 1;

  return TILE(x, y);
}

MapTile const& Map::get_tile(Vec2i tile) const
{
  return get_tile(tile.x, tile.y);
}

MapTile& Map::get_tile(Vec2i tile)
{
  return get_tile(tile.x, tile.y);
}

bool Map::tile_is_opaque(Vec2i tile)
{
  if ((tile.x < 0) || (tile.x >= m_map_size.x) || (tile.y < 0) || (tile.y >= m_map_size.y))
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
    CLOG(WARNING, "Lua") << "expected 3 arguments, got " << num_args;
    return 0;
  }

  MapId map_id = static_cast<MapId>(static_cast<unsigned int>(lua_tointeger(L, 1)));
  Vec2i coords = Vec2i(static_cast<int>(lua_tointeger(L, 2)), static_cast<int>(lua_tointeger(L, 3)));

  auto& map_tile = GAME.get_maps().get(map_id).get_tile(coords);
  ThingId contents = map_tile.get_tile_contents();

  lua_pushinteger(L, contents);

  return 1;
}

int Map::LUA_get_start_coords(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got %d" << num_args;
    return 0;
  }

  MapId map_id = static_cast<MapId>(static_cast<unsigned int>(lua_tointeger(L, 1)));

  auto& map = GAME.get_maps().get(map_id);
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
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  MapId map_id = static_cast<MapId>(static_cast<unsigned int>(lua_tointeger(L, 1)));

  auto& map = GAME.get_maps().get(map_id);

  std::string feature = lua_tostring(L, 2);

  feature_settings.set<std::string>("type", feature);

  bool success = map.m_generator->add_feature(feature_settings);

  lua_pushboolean(L, static_cast<int>(success));

  return 1;
}