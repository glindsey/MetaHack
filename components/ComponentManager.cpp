#include "stdafx.h"

#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"

int LUA_get_mass(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  int result = COMPONENTS.physical.exists(entity) ? COMPONENTS.physical[entity].mass() : 0;
  auto slot_count = the_lua_instance.push_value(result);

  return slot_count;
}

int LUA_get_opacity(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  Color result = COMPONENTS.appearance.exists(entity) ? COMPONENTS.appearance[entity].opacity() : 0;
  auto slot_count = the_lua_instance.push_value(result);

  return slot_count;
}

int LUA_get_quantity(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  unsigned int result = COMPONENTS.physical.exists(entity) ? COMPONENTS.physical[entity].quantity() : 0;
  auto slot_count = the_lua_instance.push_value(result);

  return slot_count;
}

int LUA_get_volume(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  unsigned int result = COMPONENTS.physical.exists(entity) ? COMPONENTS.physical[entity].volume() : 0;
  auto slot_count = the_lua_instance.push_value(result);

  return slot_count;
}


ComponentManager::ComponentManager()
{
  the_lua_instance.register_function("get_mass", LUA_get_mass);
  the_lua_instance.register_function("get_opacity", LUA_get_opacity);
  the_lua_instance.register_function("get_quantity", LUA_get_quantity);
  the_lua_instance.register_function("get_volume", LUA_get_volume);
}

ComponentManager::~ComponentManager()
{}

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

