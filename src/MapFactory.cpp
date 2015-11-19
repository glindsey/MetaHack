#include "MapFactory.h"

#include <boost/log/trivial.hpp>

#include "App.h"
#include "GameState.h"
#include "Map.h"

const MapId MapFactory::null_map_id = static_cast<MapId>(0);

MapFactory::MapFactory(GameState& game_state)
  :
  m_game_state{ game_state }
{
  // Create and add the "null map" to the list.
  current_map_id = 0;
  m_maps.insert(current_map_id, NEW Map(current_map_id, 1, 1));

  // Register the Map Lua functions.
  the_lua_instance.register_function("map_get_floor", Map::LUA_get_floor);
  the_lua_instance.register_function("map_get_start_coords", Map::LUA_get_start_coords);
  the_lua_instance.register_function("map_add_feature", Map::LUA_map_add_feature);
}

MapFactory::~MapFactory()
{
  //dtor
}

Map const& MapFactory::get(MapId map_id) const
{
  if (m_maps.count(map_id) == 0)
  {
    return m_maps.at(null_map_id);
  }
  else
  {
    return m_maps.at(map_id);
  }
}


Map& MapFactory::get(MapId map_id)
{
  if (m_maps.count(map_id) == 0)
  {
    return m_maps.at(null_map_id);
  }
  else
  {
    return m_maps.at(map_id);
  }
}

MapId MapFactory::create(int x, int y)
{
  ++current_map_id;
  Map* new_map = NEW Map(current_map_id, x, y);
  m_maps.insert(current_map_id, new_map);
  new_map->initialize();

  return current_map_id;
}

bool MapFactory::destroy(MapId map_id)
{
  // Can't destroy current map.
  if (map_id == current_map_id)
  {
    MAJOR_ERROR("Attempted to destroy the current map!");
    return false;
  }

  // Can't destroy current map.
  if (map_id == null_map_id)
  {
    MAJOR_ERROR("Attempted to destroy the null map!");
    return false;
  }

  if (m_maps.count(map_id) != 0)
  {
    m_maps.erase(map_id);
    return true;
  }
  else
  {
    return false;
  }
}
