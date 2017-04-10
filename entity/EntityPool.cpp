#include "stdafx.h"

#include "entity/EntityPool.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaEntityFunctions.h"

EntityPool::EntityPool(GameState& game)
  :
  m_game{ game }
{
  // Register the Entity Lua functions.
  LuaEntityFunctions::registerFunctions();

  // Create the "nothingness" object.
  EntityId mu = create("Mu");
  Assert("EntityPool", (mu == 0ULL), "Mu's ID is " << mu << " instead of zero!");

  m_initialized = true;
}

EntityPool::~EntityPool()
{
}

bool EntityPool::firstIsSubtypeOfSecond(std::string first, std::string second)
{
  //CLOG(TRACE, "Entity") << "Checking if " << first << " is a subtype of " << second << "...";

  std::string first_parent = m_game.category(first).value("parent", std::string());

  if (first_parent.empty())
  {
    //CLOG(TRACE, "Entity") << first << " parent is empty, returning false";
    return false;
  }

  if (first_parent == second)
  {
    //CLOG(TRACE, "Entity") << first << " parent = " << second << ", returning true";
    return true;
  }

  //CLOG(TRACE, "Entity") << first << " parent = " << first_parent << ", recursing...";
  return firstIsSubtypeOfSecond(first_parent, second);
}

EntityId EntityPool::create(std::string type)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = m_game.category(type);

  std::unique_ptr<Entity> new_thing{ new Entity{ m_game, type, data, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  if (m_initialized)
  {
    /// @todo Re-implement me
    m_thing_map[new_id]->call_lua_function("on_create", {}, true);
  }

  return EntityId(new_id);
}

EntityId EntityPool::createTileContents(MapTile* map_tile)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = m_game.category("TileContents");

  std::unique_ptr<Entity> new_thing{ new Entity { m_game, map_tile, "TileContents", data, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  return EntityId(new_id);
}

EntityId EntityPool::clone(EntityId original)
{
  if (this->exists(original) == false) return get_mu();
  Entity& original_thing = this->get(original);

  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;

  std::unique_ptr<Entity> new_thing{ new Entity { original_thing, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  return EntityId(new_id);
}

void EntityPool::destroy(EntityId id)
{
  if (id != get_mu())
  {
    if (m_thing_map.count(id) != 0)
    {
      m_thing_map.erase(id);
    }
  }
  else
  {
    throw std::exception("Attempted to destroy Mu object!");
  }
}

bool EntityPool::exists(EntityId id)
{
  return (m_thing_map.count(id) != 0);
}

Entity& EntityPool::get(EntityId id)
{
  try
  {
    return *(m_thing_map.at(id));
  }
  catch (std::out_of_range&)
  {
    CLOG(WARNING, "Entity") << "Tried to get entity " << id << " which does not exist";
    return *(m_thing_map[get_mu()]);
  }
}

Entity const& EntityPool::get(EntityId id) const
{
  try
  {
    return *(m_thing_map.at(id));
  }
  catch (std::out_of_range&)
  {
    CLOG(WARNING, "Entity") << "Tried to get entity " << id << " which does not exist";
    return *(m_thing_map.at(get_mu()));
  }
}

EntityId EntityPool::get_mu()
{
  return EntityId();
}