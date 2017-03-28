#include "stdafx.h"

#include "map/MapFactory.h"

#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"

const MapId MapFactory::null_map_id = static_cast<MapId>(0);

MapFactory::MapFactory(GameState& game)
  :
  m_game{ game }
{
  // Create and add the "null map" to the list.
  current_map_id = 0;
  std::unique_ptr<Map> new_map{ NEW Map { m_game, current_map_id, 1, 1 } };
  m_maps[current_map_id] = std::move(new_map);

  // Register the Map Lua functions.
  the_lua_instance.register_function("map_get_tile_contents", Map::LUA_getTileContents);
  the_lua_instance.register_function("map_get_start_coords", Map::LUA_getStartCoords);
  the_lua_instance.register_function("map_add_feature", Map::LUA_mapAddFeature);
}

MapFactory::~MapFactory()
{
  //dtor
}

bool MapFactory::exists(MapId map_id) const
{
  return (m_maps.count(map_id) != 0);
}

Map const& MapFactory::get(MapId map_id) const
{
  if (m_maps.count(map_id) == 0)
  {
    return *m_maps.at(null_map_id);
  }
  else
  {
    return *m_maps.at(map_id);
  }
}

Map& MapFactory::get(MapId map_id)
{
  if (m_maps.count(map_id) == 0)
  {
    return *m_maps.at(null_map_id);
  }
  else
  {
    return *m_maps.at(map_id);
  }
}

MapId MapFactory::create(int x, int y)
{
  ++current_map_id;

  std::unique_ptr<Map> new_map{ NEW Map{ m_game, current_map_id, x, y } };
  m_maps[current_map_id] = std::move(new_map);
  m_maps[current_map_id]->initialize();

  return current_map_id;
}

bool MapFactory::destroy(MapId map_id)
{
  // Can't destroy current map.
  if (map_id == current_map_id)
  {
    CLOG(ERROR, "MapFactory") << "attempted to destroy the current map";
    return false;
  }

  // Can't destroy current map.
  if (map_id == null_map_id)
  {
    CLOG(ERROR, "MapFactory") << "attempted to destroy the null map";
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