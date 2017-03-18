#pragma once
// Lua enum binding modified from code Copyright (c) 2010 Tom Distler.

#include "stdafx.h"

#include "ErrorHandler.h"
#include "Property.h"

/// @todo Following should be removable after legacy template methods are gone
#include "actions/ActionResult.h"
#include "Direction.h"

// Forward declarations
class EntityId;
class Property;

/// This class encapsulates the Lua state and interface as an object.
class Lua : public boost::noncopyable
{
public:
  Lua();

  virtual ~Lua();

  /// Register a function with Lua.
  void register_function(std::string name, lua_CFunction func);

  /// Execute a particular file.
  void do_file(FileName filename);

  /// Load a particular package, making sure it is not already loaded.
  void require(FileName filename, bool fatal = false);

  /// Sets a global to a particular integer value.
  /// If the global currently exists it will be overwritten.
  void set_global(std::string name, lua_Integer value);

  void stackDump() const;


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
  bool check_enum_type(const char* tname, int index) const;

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

  /// Pushes a property onto the Lua stack.
  ///
  /// Currently supported types are:
  ///  * Boolean
  ///  * String
  ///  * Number
  ///  * IntVec2
  ///  * Direction
  ///  * Color
  ///  * Property::Type
  ///
  /// @param    value   Value to deduce the type of.
  /// @return           The number of arguments pushed to the stack.
  ///                   If the type could not be deduced, it will push
  ///                   nil and return 1.
  int push_value(Property property);
  int push_value(unsigned int value);
  int push_value(int value);
  int push_value(int64_t value);
  int push_value(EntityId value);
  int push_value(float value);
  int push_value(double value);
  int push_value(bool value);
  int push_value(std::string value);
  int push_value(UintVec2 value);
  int push_value(IntVec2 value);
  int push_value(Direction value);
  int push_value(sf::Color value);
  int push_value(Property::Type value);

  /// Pop a type from a Lua function, if possible.
  /// @return The popped type, as a Property::Type.
  Property::Type pop_type();

  /// Pop a value from a Lua function, if possible.
  /// @return The popped value.
  Property pop_value(Property::Type type);

  /// Return the number of Lua stack slots associated with a particular value.
  unsigned int stack_slots(Property::Type type) const;


  /// Try to call a Lua function that takes the caller and a vector of
  /// arguments and returns a result.
  ///
  /// If the function does not exist, or it returns nil, the code attempts
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         EntityId to the entity calling the function
  /// @param args           Vector of arguments to pass to the function
  /// @param result_type    Expected return type of the function
  /// @param default_result The default result if function is not found
  ///                       after traversing the entire parent tree.
  ///                       Defaults to the default constructor of the
  ///                       return type.
  ///
  /// @return The result of the call.
  Property call_thing_function(std::string function_name,
                               EntityId caller,
                               std::vector<Property> const& args,
                               Property::Type result_type,
                               Property default_result);

  Property call_thing_function(std::string function_name,
                               EntityId caller,
                               std::vector<Property> const& args,
                               Property::Type result_type);

  /// Get a Entity group intrinsic.
  /// @param group          Name of group to get intrinsic of.
  /// @param name           Name of intrinsic to get.
  /// @param type           Type of intrinsic to get.
  /// @param default_value  Default value if intrinsic does not exist
  ///                       (defaults to default constructor of type).
  /// @return       The value of the intrinsic. 
  Property get_type_intrinsic(std::string group,
                              std::string name,
                              Property::Type type,
                              Property default_value);

  Property get_type_intrinsic(std::string group, 
                              std::string name,
                              Property::Type type);

  /// Set a Entity group intrinsic.
  /// @param group    Name of group to set intrinsic of.
  /// @param name     Name of intrinsic to set.
  /// @param value    Value to set intrinsic to.
  void set_type_intrinsic(std::string group, std::string name, Property value);

  /// Return the Lua state.
  /// @todo For cleanliness, this should not be exposed; all Lua interaction
  ///       should go through the Lua class.
  lua_State* state();

  static int LUA_trace(lua_State* L);

  /// Add the "ActionResult" enum to the Lua instance.
  /// @see ActionResult
  void addActionResultEnumToLua();

  /// Add the "Gender" enum to the Lua instance.
  /// @see Gender
  void addGenderEnumToLua();

  /// Add the "Property::Type" enum to the Lua instance.
  /// @see Property::Type
  void addPropertyTypeEnumToLua();

private:
  /// Private Lua state.
  lua_State mutable* L_;
};
