#pragma once

#include <functional>

#include "entity/EntityId.h"
#include "lua/LuaObject.h"

template <typename ReturnType>
int LUA_getValue(lua_State* L, std::function<ReturnType(EntityId)> getValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  ReturnType result = getValue(entity);
  auto slot_count = the_lua_instance.push_value(result);
  return slot_count;
}

template <typename ReturnType>
int LUA_getValueOfString(lua_State* L, std::function<ReturnType(EntityId, std::string)> getValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));
  std::string str = lua_tostring(L, 2);

  ReturnType result = getValue(entity, str);
  auto slot_count = the_lua_instance.push_value(result);
  return slot_count;
}

int LUA_setBool(lua_State* L, std::function<void(EntityId, bool)> setValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));
  bool param = static_cast<bool>(lua_toboolean(L, 2));

  setValue(entity, param);
  return 0;
}

int LUA_setInt(lua_State* L, std::function<void(EntityId, int)> setValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));
  int param = lua_tointeger(L, 2);

  setValue(entity, param);
  return 0;
}

int LUA_setFloat(lua_State* L, std::function<void(EntityId, float)> setValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));
  float param = lua_tonumber(L, 2);

  setValue(entity, param);
  return 0;
}

int LUA_setString(lua_State* L, std::function<void(EntityId, std::string)> setValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 2)
  {
    CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));
  std::string param = lua_tostring(L, 2);

  setValue(entity, param);
  return 0;
}