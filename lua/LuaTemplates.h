#pragma once

#include <functional>

#include "entity/EntityId.h"
#include "lua/LuaObject.h"

template <typename T>
int LUA_getValue(lua_State* L, std::function<T(EntityId)> getValue)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  T result = getValue(entity);
  auto slot_count = the_lua_instance.push_value(result);
  return slot_count;
}