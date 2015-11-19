// Lua enum binding modified from code Copyright (c) 2010 Tom Distler.


#ifndef LUA_H
#define LUA_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

#include "common_types.h"

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

  /// Load a particular package, making sure it is not already loaded.
  void require(std::string filename, bool fatal = false);

  /// Sets a global to a particular integer value.
  /// If the global currently exists it will be overwritten.
  void set_global(std::string name, lua_Integer value);

  /// Adds an enumerated type into Lua.
  ///
  /// L - Lua state.
  /// tname - The name of the enum type.
  /// <name:string><value:int> pairs, terminated by a null (0).
  ///
  /// EX: Assume the following enum in C-code:
  ///
  ///  typedef enum _TYPE { TYPE_FOO=0, TYPE_BAR, TYPE_BAZ, TYPE_MAX } TYPE;
  ///
  /// To map this to Lua, do the following:
  ///
  ///  add_enum_to_lua( L, "type",
  ///    "foo", TYPE_FOO,
  ///    "bar", TYPE_BAR,
  ///    "baz", TYPE_BAZ,
  ///    0);
  ///
  /// In Lua, you can access the enum as:
  ///  type.foo
  ///  type.bar
  ///  type.baz
  ///
  /// You can print the actual value in Lua by:
  ///  > print(type.foo.value)
  ///
  bool add_enum(const char* tname, ...);

  /// Validates that the specified value is the correct enumerated type.
  ///
  /// L - Lua state
  /// tname - The name of the expected type
  /// index - The stack index of the value to test
  ///
  /// EX: Assume you have a function in lua 'dosomething' which is called:
  ///
  ///  > dosomething(type.foo)
  ///
  /// Your C function gets called, and you use check_enum_type to verify the
  /// argument is the correct type.
  ///
  /// int lua_dosomething(lua_State* L)
  /// {
  ///   if (!check_enum_type(L, "type", -1))
  ///     /* error */
  ///   ...
  /// }
  ///
  bool check_enum_type(const char* tname, int index);

  /// Gets the enum value as an integer.
  ///
  /// L - Lua state
  /// index - The index on the Lua stack where the enum resides.
  ///
  /// EX:
  ///
  /// int lua_dosomething(lua_State* L)
  /// {
  ///   if (!check_enum_type(L, "type", -1))
  ///     /* error */
  ///   
  ///   int value = get_enum_value(L, -1);
  ///   ...
  /// }
  ///
  int get_enum_value(int index);

  /// Attempts to deduce the type of a boost::any and push the required
  /// values onto the Lua stack.
  /// Supported types include:
  ///  * std::string
  ///  * bool
  ///  * double
  ///  * sf::Vector2i
  ///  * sf::Color
  /// @param    value   Value to deduce the type of.
  /// @return           The number of arguments pushed to the stack.
  ///                   If the type could not be deduced, it will push
  ///                   nil and return 1.
  int push_onto_lua_stack(boost::any value);

  /// Return the Lua state.
  /// @todo For cleanliness, this should not be exposed; all Lua interaction
  ///       should go through the Lua class.
  lua_State* state();

  static int LUA_trace(lua_State* L);

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
