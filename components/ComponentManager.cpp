#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaTemplates.h"
#include "utilities/JSONUtils.h"

int LUA_get_busy_ticks(lua_State* L)
{
  return LUA_getValue<unsigned int>(L, [&](EntityId entity) -> unsigned int
  {
    return COMPONENTS.activity.existsFor(entity) ? COMPONENTS.activity[entity].busyTicks() : 0;
  });
}


int LUA_get_category(lua_State* L)
{
  return LUA_getValue<std::string>(L, [&](EntityId entity) -> std::string
  {
    return COMPONENTS.category.existsFor(entity) ? COMPONENTS.category[entity] : "";
  });
}

int LUA_get_hp(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.health.existsFor(entity) ? COMPONENTS.health[entity].hp() : 0;
  });
}

int LUA_get_mass(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].mass() : 0;
  });
}

int LUA_get_maxhp(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.health.existsFor(entity) ? COMPONENTS.health[entity].maxHp() : 0;
  });
}

int LUA_get_opacity(lua_State* L)
{
  return LUA_getValue<Color>(L, [&](EntityId entity) -> Color
  {
    return COMPONENTS.appearance.existsFor(entity) ? COMPONENTS.appearance[entity].opacity() : Color::White;
  });
}

int LUA_get_proper_name(lua_State* L)
{
  return LUA_getValue<std::string>(L, [&](EntityId entity) -> std::string
  {
    return COMPONENTS.properName.existsFor(entity) ? COMPONENTS.properName[entity] : "";
  });
}


int LUA_get_quantity(lua_State* L)
{
  return LUA_getValue<unsigned int>(L, [&](EntityId entity) -> unsigned int
  {
    return COMPONENTS.quantity.existsFor(entity) ? COMPONENTS.quantity[entity] : 1;
  });
}

int LUA_get_volume(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].volume() : 0;
  });
}

int LUA_is_lit(lua_State* L)
{
  return LUA_getValue<bool>(L, [&](EntityId entity) -> bool
  {
    return COMPONENTS.lightSource.existsFor(entity) ? COMPONENTS.lightSource[entity].lit() : false;
  });
}

int LUA_set_lit(lua_State* L)
{
  return LUA_setBool(L, [&](EntityId entity, bool value) -> void
  {
    if (COMPONENTS.lightSource.existsFor(entity)) COMPONENTS.lightSource[entity].lit() = value;
  });
}

namespace Components
{

  ComponentManager::ComponentManager(GameState& gameState) :
    m_gameState{ gameState }
  {
    initialize();
  }

  ComponentManager::ComponentManager(GameState& gameState, json const& j) :
    ComponentManager(gameState)
  {
    JSONUtils::doIfPresent(j, "globals", [this](auto const& value) { globals = value; });

    for (auto& componentPair : componentToName)
    {
      auto component = componentPair.first;
      auto& name = componentPair.second;

      JSONUtils::doIfPresent(j, name, [component](auto const& value) { *component = value; });
    }
  }

  ComponentManager::~ComponentManager()
  {}

  void ComponentManager::initialize()
  {
    m_gameState.lua().register_function("get_busy_ticks", LUA_get_busy_ticks);
    m_gameState.lua().register_function("get_category", LUA_get_category);
    m_gameState.lua().register_function("get_hp", LUA_get_hp);
    m_gameState.lua().register_function("get_mass", LUA_get_mass);
    m_gameState.lua().register_function("get_maxhp", LUA_get_maxhp);
    m_gameState.lua().register_function("get_opacity", LUA_get_opacity);
    m_gameState.lua().register_function("get_proper_name", LUA_get_proper_name);
    m_gameState.lua().register_function("get_quantity", LUA_get_quantity);
    m_gameState.lua().register_function("get_volume", LUA_get_volume);
    m_gameState.lua().register_function("is_lit", LUA_is_lit);
    m_gameState.lua().register_function("set_lit", LUA_set_lit);
  }

  void ComponentManager::clone(EntityId original, EntityId newId)
  {
    for (auto& componentPair : componentToName)
    {
      auto component = componentPair.first;
      // Clone all components except inventory.
      if (component != &inventory)
      {
        component->cloneIfExists(original, newId);
      }
    }
  }

  void ComponentManager::erase(EntityId id)
  {
    for (auto& componentPair : componentToName)
    {
      auto component = componentPair.first;
      component->remove(id);
    }
  }

  void ComponentManager::populate(EntityId id, json const& j)
  {
    for (auto& componentPair : componentToName)
    {
      auto component = componentPair.first;
      auto& name = componentPair.second;

      JSONUtils::doIfPresent(j, name, [component, id](auto const& value) { component->update(id, value); });
    }
  }

  json ComponentManager::toJSON()
  {
    json j = json::object();

    j["globals"] = globals;

    for (auto& componentPair : componentToName)
    {
      auto component = componentPair.first;
      auto& name = componentPair.second;

      j[name] = component->toJSON();
    }

    return j;
  }

  json ComponentManager::toJSON(EntityId id)
  {
    json j = json::object();

    for (auto& componentPair : componentToName)
    {
      auto component = componentPair.first;
      auto& name = componentPair.second;

      if (component->existsFor(id))
      {
        j[name] = component->toJSON(id);
      }
    }

    return j;
  }

} // end namespace
