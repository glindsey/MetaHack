#include "stdafx.h"

#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaTemplates.h"
#include "utilities/JSONUtils.h"

int LUA_get_mass(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].mass() : 0;
  });
}

int LUA_get_opacity(lua_State* L)
{
  return LUA_getValue<Color>(L, [&](EntityId entity) -> Color
  {
    return COMPONENTS.appearance.existsFor(entity) ? COMPONENTS.appearance[entity].opacity() : Color::White;
  });
}

int LUA_get_quantity(lua_State* L)
{
  return LUA_getValue<unsigned int>(L, [&](EntityId entity) -> unsigned int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].quantity() : 1;
  });
}

int LUA_get_volume(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].volume() : 0;
  });
}

ComponentManager::ComponentManager()
{
  initialize();
}

ComponentManager::ComponentManager(json const& j)
{
  initialize();

  if (j.is_object() && j.size() != 0)
  {
    JSONUtils::setIfPresent(appearance, j, "appearance");
    JSONUtils::setIfPresent(inventory, j, "inventory");
    JSONUtils::setIfPresent(physical, j, "physical");
    JSONUtils::setIfPresent(position, j, "position");
    JSONUtils::setIfPresent(spacialMemory, j, "spacial-memory");
  }

}

ComponentManager::~ComponentManager()
{}

void ComponentManager::initialize()
{
  the_lua_instance.register_function("get_mass", LUA_get_mass);
  the_lua_instance.register_function("get_opacity", LUA_get_opacity);
  the_lua_instance.register_function("get_quantity", LUA_get_quantity);
  the_lua_instance.register_function("get_volume", LUA_get_volume);
}

void ComponentManager::clone(EntityId original, EntityId newId)
{
  appearance.cloneIfExists(original, newId);
  // Do NOT clone inventory
  physical.cloneIfExists(original, newId);
  position.cloneIfExists(original, newId);
  spacialMemory.cloneIfExists(original, newId);
}

void ComponentManager::populate(EntityId id, json const& j)
{
  if (j.is_object() && j.size() != 0)
  {
    JSONUtils::setIfPresent(appearance[id], j, "appearance");
    JSONUtils::setIfPresent(inventory[id], j, "inventory");
    JSONUtils::setIfPresent(physical[id], j, "physical");
    JSONUtils::setIfPresent(position[id], j, "position");
    JSONUtils::setIfPresent(spacialMemory[id], j, "spacial-memory");
  }
}

void from_json(json const& j, ComponentManager& obj)
{
  if (j.is_object() && j.size() != 0)
  {
    JSONUtils::setIfPresent(obj.appearance, j, "appearance");
    JSONUtils::setIfPresent(obj.inventory, j, "inventory");
    JSONUtils::setIfPresent(obj.physical, j, "physical");
    JSONUtils::setIfPresent(obj.position, j, "position");
    JSONUtils::setIfPresent(obj.spacialMemory, j, "spacial-memory");
  }
}

void to_json(json& j, ComponentManager const& obj)
{
  j["appearance"] = obj.appearance;
  j["inventory"] = obj.inventory;
  j["physical"] = obj.physical;
  j["position"] = obj.position;
  j["spacial-memory"] = obj.spacialMemory;
}

