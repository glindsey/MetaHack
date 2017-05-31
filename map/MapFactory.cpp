#include "stdafx.h"

#include "map/MapFactory.h"

#include "game/App.h"
#include "game/GameState.h"
#include "map/Map.h"

MapFactory::MapFactory(GameState& gameState)
  :
  m_gameState{ gameState }
{
  // Create and add the "null map" to the list.
  m_currentMapID = "";
  std::unique_ptr<Map> new_map{ NEW Map { m_gameState, m_currentMapID, 1, 1 } };
  m_maps[m_currentMapID] = std::move(new_map);

  // Register the Map Lua functions.
  m_gameState.lua().register_function("map_get_tile_contents", Map::LUA_getTileContents);
  m_gameState.lua().register_function("map_get_start_coords", Map::LUA_getStartCoords);
  m_gameState.lua().register_function("map_add_feature", Map::LUA_mapAddFeature);
}

MapFactory::~MapFactory()
{
  //dtor
}

bool MapFactory::exists(MapID id) const
{
  return (m_maps.count(id) != 0);
}

Map const& MapFactory::get(MapID id) const
{
  if (m_maps.count(id) == 0)
  {
    return *m_maps.at("");
  }
  else
  {
    return *m_maps.at(id);
  }
}

Map& MapFactory::get(MapID id)
{
  if (m_maps.count(id) == 0)
  {
    return *m_maps.at("");
  }
  else
  {
    return *m_maps.at(id);
  }
}

MapID MapFactory::create(int x, int y)
{
  if (m_maps.count(m_currentMapID) != 0)
  {
    // Right now, just convert current map ID to number, add one, and convert
    // back to string. This will need updating later.
    if (m_currentMapID.empty())
    {
      m_currentMapID = "0";
    }
    else
    {
      auto numberMapID = std::stol(m_currentMapID);
      m_currentMapID = std::to_string(++numberMapID);
    }
  }

  m_maps.emplace(m_currentMapID, NEW Map{ m_gameState, m_currentMapID, x, y });
  m_maps[m_currentMapID]->initialize();

  return m_currentMapID;
}

bool MapFactory::destroy(MapID id)
{
  // Can't destroy current map.
  if (id == m_currentMapID)
  {
    CLOG(ERROR, "MapFactory") << "attempted to destroy the current map";
    return false;
  }

  // Can't destroy current map.
  if (id.empty())
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