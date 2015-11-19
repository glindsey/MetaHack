#include "Lua.h"

#include <sstream>
#include <boost/any.hpp>
#include <SFML/Graphics.hpp>

#include "ActionResult.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "New.h"

std::unique_ptr<Lua> Lua::instance_;

Lua::Lua()
{
  // Initialize the Lua interpreter.
  L_ = luaL_newstate();

  // Load the base libraries.
  luaL_openlibs(L_);

  /// @todo All of the other registration required... which will be a lot.
  ActionResult_add_to_lua(this);
  Gender_add_to_lua(this);

  // Register the trace function.
  register_function("print_trace", Lua::LUA_trace);

  // Run the initial Lua script.
  require("resources/default", true);
}

Lua::~Lua()
{
  /// Clean up Lua.
  lua_close(L_);
}

Lua& Lua::instance()
{
  if (Lua::instance_ == nullptr)
  {
    Lua::instance_.reset(NEW Lua());
  }

  return *(Lua::instance_.get());
}

void Lua::register_function(std::string name, lua_CFunction func)
{
  lua_register(L_, name.c_str(), func);
}

void Lua::do_file(std::string filename)
{
  const char * file = filename.c_str();
  if (luaL_dofile(L_, file))
  {
    fprintf(stderr, "%s\n", lua_tostring(L_, -1));
  }
}

void Lua::require(std::string packagename, bool fatal)
{
  int err;

  // Call the "require" function.  
  lua_getfield(L_, LUA_GLOBALSINDEX, "require"); // function
  lua_pushstring(L_, packagename.c_str());     // arg 0: module name
  err = lua_pcall(L_, 1, 1, 0);

  if (err)
  {
    if (fatal)
    {
      FATAL_ERROR("%s", lua_tostring(L_, -1));
    }
    else
    {
      MAJOR_ERROR("%s", lua_tostring(L_, -1));
    }
  }
  else
  {
    // Store module table in global var
    lua_setfield(L_, LUA_GLOBALSINDEX, packagename.c_str());
  }
}


void Lua::set_global(std::string name, lua_Integer value)
{
  lua_pushinteger(L_, value);
  lua_setglobal(L_, name.c_str());
}

bool Lua::add_enum(const char* tname, ...)
{
  // NOTE: Here's the Lua code we're building and executing to define the
  //       enum.
  //
  // <tname> = setmetatable( {}, { 
  //      __index = { 
  //          <name1> = { 
  //              value = <value1>, 
  //              type = \"<tname>\"
  //          }, 
  //          ... 
  //      },
  //      __newindex = function(table, key, value)
  //          error(\"Attempt to modify read-only table\")
  //      end,
  //      __metatable = false
  // });

  va_list args;
  std::stringstream code;
  char* ename;
  int evalue;

  code << tname << " = setmetatable({}, {";
  code << "__index = {";

  // Iterate over the variadic arguments adding the enum values.
  va_start(args, tname);
  while ((ename = va_arg(args, char*)) != 0)
  {
    evalue = va_arg(args, int);
    code << ename << "={value=" << evalue << ",type=\"" << tname << "\"},";
  }
  va_end(args);

  code << "},";
  code << "__newindex = function(table, key, value) error(\"Attempt to modify read-only table\") end,";
  code << "__metatable = false});";

  // Execute lua code
  if (luaL_loadbuffer(L_, code.str().c_str(), code.str().length(), 0) || lua_pcall(L_, 0, 0, 0))
  {
    fprintf(stderr, "%s\n", lua_tostring(L_, -1));
    lua_pop(L_, 1);
    return false;
  }
  return true;
}

bool Lua::check_enum_type(const char* tname, int index)
{
  lua_pushstring(L_, "type");
  lua_gettable(L_, index - 1);
  if (!lua_isnil(L_, -1))
  {
    const char* type = lua_tostring(L_, -1);
    if (strcmp(type, tname) == 0)
    {
      lua_pop(L_, 1);
      return true;
    }
  }
  lua_pop(L_, 1);
  return false;
}

int Lua::get_enum_value(int index)
{
  int value;
  lua_pushstring(L_, "value");
  lua_gettable(L_, index - 1);
  value = static_cast<int>(luaL_checkinteger(L_, -1));
  lua_pop(L_, 1);
  return value;
}

int Lua::push_onto_lua_stack(boost::any value)
{
  if (boost::any_cast<std::string>(&value))
  {
    std::string cast_value = boost::any_cast<std::string>(value);
    lua_pushstring(L_, cast_value.c_str());
    return 1;
  }
  else if (boost::any_cast<bool>(&value))
  {
    bool cast_value = boost::any_cast<bool>(value);
    lua_pushboolean(L_, cast_value);
    return 1;
  }
  else if (boost::any_cast<double>(&value))
  {
    double cast_value = boost::any_cast<double>(value);
    lua_pushnumber(L_, static_cast<lua_Number>(cast_value));
    return 1;
  }
  else if (boost::any_cast<sf::Vector2i>(&value))
  {
    sf::Vector2i cast_value = boost::any_cast<sf::Vector2i>(value);
    lua_pushinteger(L_, cast_value.x);
    lua_pushinteger(L_, cast_value.y);
    return 2;
  }
  else if (boost::any_cast<sf::Color>(&value))
  {
    sf::Color cast_value = boost::any_cast<sf::Color>(value);
    lua_pushinteger(L_, cast_value.r);
    lua_pushinteger(L_, cast_value.g);
    lua_pushinteger(L_, cast_value.b);
    lua_pushinteger(L_, cast_value.a);
    return 4;
  }
  else
  {
    lua_pushnil(L_);
    return 1;
  }
}

lua_State* Lua::state()
{
  return L_;
}

int Lua::LUA_trace(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    MINOR_ERROR("Expected 1 argument, got %d", num_args);
  }
  else
  {
    std::string str = lua_tostring(L, 1);
    TRACE(str.c_str());
  }

  return 0;
}
