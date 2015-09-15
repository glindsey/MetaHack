#include "Lua.h"

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

lua_State* Lua::state()
{
  return L_;
}

