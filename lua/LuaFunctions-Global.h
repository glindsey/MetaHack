#pragma once

// === MACROS =================================================================
#define STRINGIFY(x) #x
#define LUA_REGISTER(x) do { lua.register_function(STRINGIFY(x), &x); } while(0)

// Lua includes
extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "AssertHelper.h"
#include "game/App.h"
#include "lua/LuaObject.h"
#include "services/Service.h"
#include "services/IConfigSettings.h"
#include "services/IMessageLog.h"
#include "systems/Manager.h"
#include "systems/SystemLuaLiaison.h"

namespace LuaFunctions
{
  int get_config(lua_State* L)
  {
    int num_args = lua_gettop(L);
    auto& config = Service<IConfigSettings>::get();

    if (num_args != 1)
    {
      CLOG(WARNING, "ConfigSettings") << "expected 1 arguments, got " << num_args;
      return 0;
    }

    const char* key = lua_tostring(L, 1);

    /// @todo Re-implement me
    //if (!config.contains(key))
    //{
    lua_pushnil(L);
    return 1;
    //}
    //else
    //{
    //  auto result = config.get(key);
    //  int args = instance().m_lua->push_value(result);
    //  return args;
    //}
  }

  int get_frame_counter(lua_State* L)
  {
    auto& app = App::instance();

    int num_args = lua_gettop(L);

    if (num_args != 0)
    {
      CLOG(WARNING, "App") << "expected 0 arguments, got " << num_args;
      return 0;
    }

    lua_pushinteger(L, app.frameCounter());

    return 1;
  }

  int message(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "App") << "Expected 1 argument, got " << num_args;
    }
    else
    {
      std::string str = lua_tostring(L, 1);
      Service<IMessageLog>::get().add(str);
    }

    return 0;
  }

  int redirect_print(lua_State* L)
  {
    int nargs = lua_gettop(L);

    for (int i = 1; i <= nargs; i++)
    {
      if (lua_isstring(L, i))
      {
        std::string str = lua_tostring(L, i);
        Service<IMessageLog>::get().add(str);
      }
      else
      {
        /* Do something with non-strings if you like */
      }
    }

    return 0;
  }

  void registerFunctionsGlobal(Lua& lua)
  {
    LUA_REGISTER(get_config);
    LUA_REGISTER(get_frame_counter);
    LUA_REGISTER(message);
    LUA_REGISTER(redirect_print);
  }

} // end namespace