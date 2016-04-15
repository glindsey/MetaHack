// Lua enum binding modified from code Copyright (c) 2010 Tom Distler.

#ifndef LUA_H
#define LUA_H

#include "stdafx.h"

#include "ErrorHandler.h"
#include "ThingRef.h"

/// Forward declarations
enum class ActionResult;

/// Global Lua interface
class Lua : public boost::noncopyable
{
public:
  virtual ~Lua();

  /// Get the Lua instance.
  static Lua& instance();

  /// Register a function with Lua.
  void register_function(StringKey name, lua_CFunction func);

  /// Execute a particular file.
  void do_file(FileName filename);

  /// Load a particular package, making sure it is not already loaded.
  void require(FileName filename, bool fatal = false);

  /// Sets a global to a particular integer value.
  /// If the global currently exists it will be overwritten.
  void set_global(StringKey name, lua_Integer value);

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

  /// Push a value to a Lua function, if possible.
  ///
  /// @param value    Value to push.
  /// @return The number of stack slots used.
  template < typename T > int push_value(T value) { return 0; }

  /// Attempts to deduce the type of a boost::any and push the required
  /// values onto the Lua stack.
  /// Supported types include:
  ///  * StringKey
  ///  * bool
  ///  * double
  ///  * sf::Vector2i
  ///  * sf::Color
  /// @param    value   Value to deduce the type of.
  /// @return           The number of arguments pushed to the stack.
  ///                   If the type could not be deduced, it will push
  ///                   nil and return 1.
  template <> int push_value(boost::any value)
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

  template <> int push_value(unsigned int value)
  {
    lua_pushinteger(L_, static_cast<lua_Integer>(value));
    return 1;
  }

  template <> int push_value(int value)
  {
    lua_pushinteger(L_, static_cast<lua_Integer>(value));
    return 1;
  }

  template <> int push_value(float value)
  {
    lua_pushnumber(L_, static_cast<lua_Number>(value));
    return 1;
  }

  template <> int push_value(double value)
  {
    lua_pushnumber(L_, static_cast<lua_Number>(value));
    return 1;
  }

  template <> int push_value(bool value)
  {
    lua_pushboolean(L_, static_cast<int>(value));
    return 1;
  }

  template <> int push_value(std::string value)
  {
    lua_pushstring(L_, value.c_str());
    return 1;
  }

  template <> int push_value(sf::Vector2u value)
  {
    lua_pushnumber(L_, static_cast<lua_Integer>(value.x));
    lua_pushnumber(L_, static_cast<lua_Integer>(value.y));
    return 2;
  }

  template <> int push_value(sf::Color value)
  {
    lua_pushnumber(L_, static_cast<lua_Integer>(value.r));
    lua_pushnumber(L_, static_cast<lua_Integer>(value.g));
    lua_pushnumber(L_, static_cast<lua_Integer>(value.b));
    lua_pushnumber(L_, static_cast<lua_Integer>(value.a));
    return 4;
  }

  /// Pop a value from a Lua function, if possible.
  ///
  /// @return The popped value.
  template < typename T > T pop_value();

  template <> void pop_value()
  {}

  template <> unsigned int pop_value()
  {
    unsigned int return_value = lua_tointeger(L_, -1);
    lua_pop(L_, 1);
    return return_value;
  }

  template <> int pop_value()
  {
    int return_value = lua_tointeger(L_, -1);
    lua_pop(L_, 1);
    return return_value;
  }

  template <> float pop_value()
  {
    float return_value = static_cast<float>(lua_tonumber(L_, -1));
    lua_pop(L_, 1);
    return return_value;
  }

  template <> double pop_value()
  {
    double return_value = lua_tonumber(L_, -1);
    lua_pop(L_, 1);
    return return_value;
  }

  template <> bool pop_value()
  {
    bool return_value = (lua_toboolean(L_, -1) != 0);
    lua_pop(L_, 1);
    return return_value;
  }

  template <> std::string pop_value()
  {
    std::string return_value;
    char const* return_ptr = lua_tostring(L_, -1);
    if (return_ptr != nullptr)
    {
      return_value = std::string(return_ptr);
    }
    lua_pop(L_, 1);
    return return_value;
  }

  template <> sf::Vector2u pop_value()
  {
    sf::Vector2u return_value = sf::Vector2u(lua_tointeger(L_, -2),
                                             lua_tointeger(L_, -1));
    lua_pop(L_, 2);
    return return_value;
  }

  template <> sf::Color pop_value()
  {
    sf::Color return_value = sf::Color(lua_tointeger(L_, -4),
                                       lua_tointeger(L_, -3),
                                       lua_tointeger(L_, -2),
                                       lua_tointeger(L_, -1));
    lua_pop(L_, 4);
    return return_value;
  }

  template <> ActionResult pop_value()
  {
    ActionResult return_value = static_cast<ActionResult>(get_enum_value(-1));
    lua_pop(L_, 1);
    return return_value;
  }

  /// Return the number of Lua stack slots associated with a particular value.
  template < typename T > unsigned int stack_slots();
  template<> unsigned int stack_slots<void>() { return 0; }
  template<> unsigned int stack_slots<unsigned int>() { return 1; }
  template<> unsigned int stack_slots<int>() { return 1; }
  template<> unsigned int stack_slots<float>() { return 1; }
  template<> unsigned int stack_slots<double>() { return 1; }
  template<> unsigned int stack_slots<bool>() { return 1; }
  template<> unsigned int stack_slots<std::string>() { return 1; }
  template<> unsigned int stack_slots<sf::Vector2u>() { return 2; }
  template<> unsigned int stack_slots<sf::Color>() { return 4; }
  template<> unsigned int stack_slots<ActionResult>() { return 1; }

  /// Try to call a Lua function that takes the caller and a vector of
  /// arguments and returns a result.
  ///
  /// If the function does not exist, or it returns nil, the code attempts
  /// the code attempts to step up to the parent type and call the function
  /// there.
  ///
  /// @param function_name  Name of the function to call
  /// @param caller         ThingRef to the thing calling the function
  /// @param args           Vector of arguments to pass to the function
  /// @param default_result The default result if function is not found
  ///                       after traversing the entire parent tree.
  ///                       Defaults to the default constructor of the
  ///                       return type.
  ///
  /// @return The result of the call.
  template < typename ResultType, typename ArgType >
  ResultType call_thing_function(std::string function_name,
                                 ThingRef caller,
                                 std::vector<ArgType> const& args,
                                 ResultType default_result = ResultType())
  {
    ResultType return_value = default_result;
    StringKey name = caller->get_type();

    int start_stack = lua_gettop(L_);

    // Push name of class onto the stack. (+1)
    lua_getglobal(L_, name.c_str());

    if (lua_isnoneornil(L_, -1))
    {
      // Class not found -- pop the name back off. (-1)
      lua_pop(L_, 1);
    }
    else
    {
      // Push name of function onto the stack. (+1)
      lua_getfield(L_, -1, function_name.c_str());

      if (lua_isnoneornil(L_, -1))
      {
        // Function not found -- pop the function and class names back off. (-2)
        lua_pop(L_, 2);

        CLOG(WARNING, "Lua") << "Could not find Lua function " << name << ":" << function_name;
      }
      else
      {
        // Remove the class name from the stack. (-1)
        lua_remove(L_, -2);

        // Push the caller's ID onto the stack. (+1)
        lua_pushinteger(L_, caller);

        // Push the arguments onto the stack. (+N)
        unsigned int lua_args = 0;
        for (auto& arg : args)
        {
          lua_args += stack_slots<ArgType>();
          push_value(arg);
        }

        // Call the function with N+1 arguments and R results. (-(N+2), +R) = R-(N+2)
        int result = lua_pcall(L_, lua_args + 1, stack_slots<ResultType>(), 0);
        if (result == 0)
        {
          // Pop the return value off of the stack. (-R)
          return_value = pop_value<ResultType>();
        }
        else
        {
          // Get the error message.
          char const* error_message = lua_tostring(L_, -1);
          MAJOR_ERROR("Error calling %s.%s: %s", name.c_str(), function_name.c_str(), error_message);

          // Pop the error message off the stack. (-1)
          lua_pop(L_, 1);
        }
      }
    }

    int end_stack = lua_gettop(L_);

    if (start_stack != end_stack)
    {
      FATAL_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
    }

    return return_value;
  }

  /// Get a Thing type intrinsic.
  /// @param type           Name of type to get intrinsic of.
  /// @param name           Name of intrinsic to get.
  /// @param default_value  Default value if intrinsic does not exist
  ///                       (defaults to default constructor of type).
  /// @return       The value of the intrinsic.
  template < typename ReturnType >
  ReturnType Lua::get_type_intrinsic(StringKey type, StringKey name, ReturnType default_value = ReturnType())
  {
    ReturnType return_value = default_value;

    int start_stack = lua_gettop(L_);

    // Push type of class onto the stack. (+1)
    lua_getglobal(L_, type.c_str());           // class <

    if (lua_isnoneornil(L_, -1))
    {
      // Class not found -- pop the name back off. (-1)
      lua_pop(L_, 1);
      MAJOR_ERROR("Lua class %s was not found", type.c_str());
    }
    else
    {
      // Push name of function onto the stack. (+1)
      lua_getfield(L_, -1, "get_intrinsic");             // class get <

      if (lua_isnoneornil(L_, -1))
      {
        // Function not found -- pop the function and class names back off. (-2)
        lua_pop(L_, 2);
        MAJOR_ERROR("Lua function %s:get_intrinsic() was not found", type.c_str());
      }
      else
      {
        // Push the class name up to the front of the stack.
        lua_pushvalue(L_, -2);                 // class get class <
        lua_remove(L_, -3);                    // get class <
        lua_pushstring(L_, name.c_str());      // get class name <

        int num_results = stack_slots<ReturnType>();
        int result = lua_pcall(L_, 2, num_results, 0);   // (result|nil|err) <
        if (result == 0)
        {
          return_value = pop_value<ReturnType>();
        }
        else
        {
          // Get the error message.
          char const* error_message = lua_tostring(L_, -1);
          MAJOR_ERROR("Error calling %s:get_intrinsic(%s): %s", type.c_str(), name.c_str(), error_message);

          // Pop the error message off the stack. (-1)
          lua_pop(L_, 1);
        }
      }
    }

    int end_stack = lua_gettop(L_);

    if (start_stack != end_stack)
    {
      FATAL_ERROR("*** LUA STACK MISMATCH (%s:get_intrinsic): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
    }

    return return_value;
  }

  /// Set a Thing type intrinsic.
  /// @param type   Name of type to set intrinsic of.
  /// @param name   Name of intrinsic to set.
  /// @param value  Value to set intrinsic to.
  template < typename ValueType >
  void set_type_intrinsic(StringKey type, StringKey name, ValueType value)
  {
    int start_stack = lua_gettop(L_);

    // Push type of class onto the stack. (+1)
    lua_getglobal(L_, type.c_str());           // class <

    if (lua_isnoneornil(L_, -1))
    {
      // Class not found -- pop the name back off. (-1)
      lua_pop(L_, 1);
      MAJOR_ERROR("Lua class %s was not found", type.c_str());
    }
    else
    {
      // Push name of function onto the stack. (+1)
      lua_getfield(L_, -1, "set_intrinsic");             // class get <

      if (lua_isnoneornil(L_, -1))
      {
        // Function not found -- pop the function and class names back off. (-2)
        lua_pop(L_, 2);
        MAJOR_ERROR("Lua function %s:set_intrinsic() was not found", type.c_str());
      }
      else
      {
        // Push the class name up to the front of the stack.
        lua_pushvalue(L_, -2);                         // class set class <
        lua_remove(L_, -3);                            // set class <
        lua_pushstring(L_, name.c_str());              // set class name <
        push_value<ValueType>(value);                  // set class name value <
        int num_args = 2 + stack_slots<ValueType>();
        int result = lua_pcall(L_, num_args, 0, 0);    // (nil|err) <
        if (result != 0)
        {
          // Get the error message.
          char const* error_message = lua_tostring(L_, -1);
          MAJOR_ERROR("Error calling %s:set_intrinsic(%s): %s", type.c_str(), name.c_str(), error_message);

          // Pop the error message off the stack. (-1)
          lua_pop(L_, 1);
        }
      }
    }

    int end_stack = lua_gettop(L_);

    if (start_stack != end_stack)
    {
      FATAL_ERROR("*** LUA STACK MISMATCH (%s:set_intrinsic): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
    }
  }

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
