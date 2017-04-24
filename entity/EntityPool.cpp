#include "stdafx.h"

#include "entity/EntityPool.h"

#include "components/ComponentManager.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaEntityFunctions.h"
#include "Service.h"
#include "services/IGameRules.h"

EntityPool::EntityPool(GameState& state)
  :
  m_state{ state }
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

EntityId EntityPool::create(std::string category)
{
  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;
  json& data = Service<IGameRules>::get().category(category);

  std::unique_ptr<Entity> new_thing{ new Entity{ m_state, category, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  auto& jsonComponents = data["components"];
  COMPONENTS.populate(new_id, jsonComponents);

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
  json& data = Service<IGameRules>::get().category("TileContents");

  std::unique_ptr<Entity> new_thing{ new Entity { m_state, map_tile, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  auto& jsonComponents = data["components"];
  COMPONENTS.populate(new_id, jsonComponents);

  return EntityId(new_id);
}

EntityId EntityPool::clone(EntityId original)
{
  if (this->exists(original) == false) return EntityId::Mu();
  Entity& original_thing = this->get(original);

  EntityId new_id = EntityId(m_nextEntityId);
  ++m_nextEntityId;

  std::unique_ptr<Entity> new_thing{ new Entity { original_thing, new_id } };
  m_thing_map[new_id] = std::move(new_thing);

  return EntityId(new_id);
}

void EntityPool::destroy(EntityId id)
{
  if (id != EntityId::Mu())
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
    return *(m_thing_map[EntityId::Mu()]);
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
    return *(m_thing_map.at(EntityId::Mu()));
  }
}