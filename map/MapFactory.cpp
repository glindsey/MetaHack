#include "stdafx.h"

#include "map/MapFactory.h"

#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"

const MapId MapFactory::nullMapId = static_cast<MapId>(0);

MapFactory::MapFactory(GameState& gameState)
  :
  m_gameState{ gameState }
{
  // Create and add the "null map" to the list.
  m_currentMapId = 0;
  std::unique_ptr<Map> new_map{ NEW Map { m_gameState, m_currentMapId, 1, 1 } };
  m_maps[m_currentMapId] = std::move(new_map);

  // Register the Map Lua functions.
  m_gameState.lua().register_function("map_get_tile_contents", Map::LUA_getTileContents);
  m_gameState.lua().register_function("map_get_start_coords", Map::LUA_getStartCoords);
  m_gameState.lua().register_function("map_add_feature", Map::LUA_mapAddFeature);
}

MapFactory::~MapFactory()
{
  //dtor
}

bool MapFactory::exists(MapId id) const
{
  return (m_maps.count(id) != 0);
}

Map const& MapFactory::get(MapId id) const
{
  if (m_maps.count(id) == 0)
  {
    return *m_maps.at(nullMapId);
  }
  else
  {
    return *m_maps.at(id);
  }
}

Map& MapFactory::get(MapId id)
{
  if (m_maps.count(id) == 0)
  {
    return *m_maps.at(nullMapId);
  }
  else
  {
    return *m_maps.at(id);
  }
}

MapId MapFactory::create(int x, int y)
{
  ++m_currentMapId;

  std::unique_ptr<Map> new_map{ NEW Map{ m_gameState, m_currentMapId, x, y } };
  m_maps[m_currentMapId] = std::move(new_map);
  m_maps[m_currentMapId]->initialize();

  return m_currentMapId;
}

bool MapFactory::destroy(MapId id)
{
  // Can't destroy current map.
  if (id == m_currentMapId)
  {
    CLOG(ERROR, "MapFactory") << "attempted to destroy the current map";
    return false;
  }

  // Can't destroy current map.
  if (id == nullMapId)
  {
    CLOG(ERROR, "MapFactory") << "attempted to destroy the null map";
    return false;
  }

  if (m_maps.count(id) != 0)
  {
    m_maps.erase(id);
    return true;
  }
  else
  {
    return false;
  }
}