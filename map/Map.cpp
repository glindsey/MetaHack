#include "stdafx.h"

#include "map/Map.h"

#include "AssertHelper.h"
#include "components/ComponentInventory.h"
#include "components/ComponentManager.h"
#include "config/Paths.h"
#include "game/App.h"
#include "game/GameState.h"
#include "entity/EntityFactory.h"
#include "maptile/MapTile.h"
#include "services/Service.h"
#include "services/IGameRules.h"
#include "types/Color.h"
#include "types/LightInfluence.h"
#include "types/ShaderEffect.h"
#include "utilities/MathUtils.h"
#include "utilities/RNGUtils.h"

#include "map/MapFeature.h"
#include "map/MapGenerator.h"

#define VERTEX(x, y) (20 * (m_size.x * y) + x)
#define TILE(x, y) (m_tiles->get({ x, y }))

/// @todo Have this take an IntVec2 instead of width x height
Map::Map(GameState& gameState, MapID id, int width, int height)
  :
  Object({}),
  m_gameState{ gameState },
  m_id{ id },
  m_size{ width, height },
  m_generator{ NEW MapGenerator(*this) }
{
  CLOG(TRACE, "Map") << "Creating map of size " << width << " x " << height;

  m_tiles.reset(NEW Grid2D<MapTile>({ width, height },
                                    [&](IntVec2 coords) -> MapTile*
  {
    MapTile* new_tile = NEW MapTile(coords, id,
                                    gameState.entities(),
                                    gameState.components());
    return new_tile;
  }));

  CLOG(TRACE, "Map") << "Map created.";

  //notifyObservers(Event::Updated);
}

void Map::initialize()
{
  // If the map isn't the 1x1 "limbo" map...
  if ((m_size.x != 1) && (m_size.y != 1))
  {
    // Generate the map.
    m_generator->generate();

    // Run Lua script associated with this map.
    /// @todo Different scripts for different maps.
    ///       And for that matter, ALL of map generation
    ///       should be done via scripting. But for now
    ///       this will do.
    CLOG(TRACE, "Map") << "Executing Map Lua script.";
    m_gameState.lua().set_global("current_map_id", m_id);
    m_gameState.lua().require(Config::paths().resources() + "/script/map");
  }

  CLOG(TRACE, "Map") << "Map initialized.";

  //notifyObservers(Event::Updated);
}

Map::~Map()
{
  //dtor
}

int Map::getIndex(IntVec2 coords) const
{
  return (coords.y * m_size.x) + coords.x;
}

bool Map::isInBounds(IntVec2 coords) const
{
  return ((coords.x >= 0) && (coords.y >= 0) &&
    (coords.x < m_size.x) && (coords.y < m_size.y));
}

bool Map::calcCoords(IntVec2 origin,
                     Direction direction,
                     IntVec2& result)
{
  result = origin + (IntVec2)direction;

  return isInBounds(result);
}

MapID Map::getMapID() const
{
  return m_id;
}

IntVec2 const& Map::getSize() const
{
  return m_size;
}

IntVec2 const& Map::getStartCoords() const
{
  return m_start_coords;
}

bool Map::setStartCoords(IntVec2 start_coords)
{
  if (isInBounds(start_coords))
  {
    m_start_coords = start_coords;
    return true;
  }
  return false;
}

MapTile const& Map::getTile(IntVec2 tile) const
{
  if (tile.x < 0) tile.x = 0;
  if (tile.x >= m_size.x) tile.x = m_size.x - 1;
  if (tile.y < 0) tile.y = 0;
  if (tile.y >= m_size.y) tile.y = m_size.y - 1;

  return TILE(tile.x, tile.y);
}

MapTile& Map::getTile(IntVec2 tile)
{
  if (tile.x < 0) tile.x = 0;
  if (tile.x >= m_size.x) tile.x = m_size.x - 1;
  if (tile.y < 0) tile.y = 0;
  if (tile.y >= m_size.y) tile.y = m_size.y - 1;

  return TILE(tile.x, tile.y);
}

bool Map::tileIsOpaque(IntVec2 tile) const
{
  if ((tile.x < 0) || (tile.x >= m_size.x) || (tile.y < 0) || (tile.y >= m_size.y))
  {
    return true;
  }

  return TILE(tile.x, tile.y).isTotallyOpaque();
}

void Map::clearMapFeatures()
{
  m_features.clear();
}

MapFeature& Map::getRandomMapFeature()
{
  Assert("MapGenerator", m_features.size() >= 1, "getRandomMapFeature() called but map doesn't contain any features yet!");

  int featureIndex = the_RNG.pick_uniform(0, static_cast<int>(m_features.size() - 1));
  return m_features[featureIndex];
}

boost::ptr_deque<MapFeature> const& Map::getMapFeatures() const
{
  return m_features;
}

MapFeature& Map::addMapFeature(MapFeature* feature)
{
  if (feature != nullptr)
  {
    m_features.push_back(feature);
  }
  return *feature;
}

int Map::LUA_getTileContents(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 3)
  {
    CLOG(WARNING, "Lua") << "expected 3 arguments, got " << num_args;
    return 0;
  }

  MapID map_id = MapID(lua_tostring(L, 1));
  IntVec2 coords = IntVec2(static_cast<int>(lua_tointeger(L, 2)), static_cast<int>(lua_tointeger(L, 3)));

  auto& map_tile = GameState::instance().maps().get(map_id).getTile(coords);
  EntityId contents = map_tile.getSpaceEntity();

  lua_pushinteger(L, contents);

  return 1;
}

int Map::LUA_getStartCoords(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got %d" << num_args;
    return 0;
  }

  MapID map_id = MapID(lua_tostring(L, 1));

  auto& map = GameState::instance().maps().get(map_id);
  auto coords = map.getStartCoords();

  lua_pushinteger(L, coords.x);
  lua_pushinteger(L, coords.y);

  return 2;
}

int Map::LUA_mapAddFeature(lua_State* L)
{
  PropertyDictionary feature_settings;

  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  MapID map_id = MapID(lua_tostring(L, 1));

  auto& map = GameState::instance().maps().get(map_id);

  std::string feature = lua_tostring(L, 2);

  feature_settings.set("type", feature);

  bool success = map.m_generator->add_feature(feature_settings);

  lua_pushboolean(L, static_cast<int>(success));

  return 1;
}
