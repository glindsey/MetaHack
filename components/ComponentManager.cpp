#include "stdafx.h"

#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaTemplates.h"

int LUA_get_mass(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.exists(entity) ? COMPONENTS.physical[entity].mass() : 0;
  });
}

int LUA_get_opacity(lua_State* L)
{
  return LUA_getValue<Color>(L, [&](EntityId entity) -> Color
  {
    return COMPONENTS.appearance.exists(entity) ? COMPONENTS.appearance[entity].opacity() : Color::White;
  });
}

int LUA_get_quantity(lua_State* L)
{
  return LUA_getValue<unsigned int>(L, [&](EntityId entity) -> unsigned int
  {
    return COMPONENTS.physical.exists(entity) ? COMPONENTS.physical[entity].quantity() : 1;
  });
}

int LUA_get_volume(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.exists(entity) ? COMPONENTS.physical[entity].volume() : 0;
  });
}

ComponentManager::ComponentManager()
{
  initialize();
}

ComponentManager::ComponentManager(json const& j)
{
  appearance = j["appearance"];
  physical = j["physical"];
  position = j["position"];

  initialize();
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

void ComponentManager::populate(EntityId id, json const& j)
{
  if (j.count("appearance") != 0) appearance[id] = j["appearance"];
  if (j.count("physical") != 0) physical[id] = j["physical"];
  if (j.count("position") != 0) position[id] = j["position"];
}

void from_json(json const& j, ComponentManager& obj)
{
  obj.appearance = j.value("appearance", json::object());
  obj.physical = j.value("physical", json::object());
  obj.position = j.value("position", json::object());
}

void to_json(json& j, ComponentManager const& obj)
{
  j["appearance"] = obj.appearance;
  j["physical"] = obj.physical;
  j["position"] = obj.position;
}

