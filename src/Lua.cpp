#include "Lua.h"

#include <sstream>

std::unique_ptr<Lua> Lua::instance_;

Lua::Lua()
{
  // Initialize the Lua interpreter.
  L_ = luaL_newstate();

  // Load the base libraries.
  luaL_openlibs(L_);

  /// @todo All of the other registration required... which will be a lot.

  // Run the initial Lua script.
  luaL_dofile(L_, "resources/default.lua");
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
    Lua::instance_.reset(new Lua());
  }

  return *(Lua::instance_.get());
}

void Lua::register_function(std::string name, lua_CFunction func)
{
  lua_register(L_, name.c_str(), func);
}

void Lua::do_file(std::string filename)
{
  luaL_dofile(L_, filename.c_str());
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
  lua_pushstring(L_, "value");
  lua_gettable(L_, index - 1);
  return (int)luaL_checkinteger(L_, -1);
}

lua_State* Lua::state()
{
  return L_;
}

