#ifndef LUA_H
#define LUA_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

/// Global Lua interface

class Lua : public boost::noncopyable
{
public:
  virtual ~Lua();

  /// Get the Lua instance.
  static Lua& instance();

  /// Register a function with Lua.
  void register_function(std::string name, lua_CFunction func);

  /// Execute a particular file.
  void do_file(std::string filename);

  /// Return the Lua state.
  lua_State* state();

private:
  /// Constructor (private because this is a singleton).
  Lua();

  /// Private Lua state.
  lua_State* L_;

  /// Unique pointer to singleton instance.
  static std::unique_ptr<Lua> instance_;
};

#define the_lua_instance (Lua::instance())
#define the_lua_state   (Lua::instance().state())

#endif // LUA_H
