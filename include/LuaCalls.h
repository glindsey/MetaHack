#ifndef LUACALLS_H
#define LUACALLS_H

#include "stdafx.h"

#include "ActionResult.h"
#include "LuaObject.h"
#include "ThingRef.h"

/// Push a value to a Lua function, if possible.
///
/// @param value    Value to push.
template < typename T > void push_value_to_lua(T value) {}

template <> void push_value_to_lua(unsigned int value);
template <> void push_value_to_lua(int value);
template <> void push_value_to_lua(float value);
template <> void push_value_to_lua(double value);
template <> void push_value_to_lua(bool value);
template <> void push_value_to_lua(std::string value);
template <> void push_value_to_lua(sf::Vector2u value);
template <> void push_value_to_lua(sf::Color value);

/// Pop a value from a Lua function, if possible.
///
/// @return The popped value.
template < typename T >T pop_value_from_lua();

template <> void pop_value_from_lua();
template <> unsigned int pop_value_from_lua();
template <> int pop_value_from_lua();
template <> float pop_value_from_lua();
template <> double pop_value_from_lua();
template <> bool pop_value_from_lua();
template <> std::string pop_value_from_lua();
template <> sf::Vector2u pop_value_from_lua();
template <> sf::Color pop_value_from_lua();
template <> ActionResult pop_value_from_lua();

/// Return the number of Lua stack slots associated with a particular value.
template < typename T > unsigned int lua_stack_slots();
template<> unsigned int lua_stack_slots<void>();
template<> unsigned int lua_stack_slots<unsigned int>();
template<> unsigned int lua_stack_slots<int>();
template<> unsigned int lua_stack_slots<float>();
template<> unsigned int lua_stack_slots<double>();
template<> unsigned int lua_stack_slots<bool>();
template<> unsigned int lua_stack_slots<std::string>();
template<> unsigned int lua_stack_slots<sf::Vector2u>();
template<> unsigned int lua_stack_slots<sf::Color>();
template<> unsigned int lua_stack_slots<ActionResult>();

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
ResultType call_lua_thing_function(std::string function_name,
                                   ThingRef caller,
                                   std::vector<ArgType> const& args,
                                   ResultType default_result = ResultType())
{
  ResultType return_value = default_result;
  StringKey name = caller->get_type();

  int start_stack = lua_gettop(the_lua_state);

  // Push name of class onto the stack. (+1)
  lua_getglobal(the_lua_state, name.c_str());

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, function_name.c_str());

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
    }
    else
    {
      // Remove the class name from the stack. (-1)
      lua_remove(the_lua_state, -2);

      // Push the caller's ID onto the stack. (+1)
      lua_pushinteger(the_lua_state, caller);

      // Push the arguments onto the stack. (+N)
      unsigned int lua_args = 0;
      for (auto& arg : args)
      {
        lua_args += lua_stack_slots<ArgType>();
        push_value_to_lua(arg);
      }

      // Call the function with N+1 arguments and R results. (-(N+2), +R) = R-(N+2)
      int result = lua_pcall(the_lua_state, lua_args + 1, lua_stack_slots<ResultType>(), 0);
      if (result == 0)
      {
        //if (!lua_istable(the_lua_state, -1))
        //{
        //  FATAL_ERROR("Expected ActionResult from Lua function but got %s", lua_typename(the_lua_state, lua_type(the_lua_state, -1)));
        //}

        // Pop the return value off of the stack. (-R)
        return_value = pop_value_from_lua<ResultType>();
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s.%s: %s", name.c_str(), function_name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

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
ReturnType get_type_intrinsic(StringKey type, StringKey name, ReturnType default_value = ReturnType())
{
  ReturnType return_value = default_value;

  int start_stack = lua_gettop(the_lua_state);

  // Push type of class onto the stack. (+1)
  lua_getglobal(the_lua_state, type.c_str());           // class <

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    MAJOR_ERROR("Lua class %s was not found", type.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, "get_intrinsic");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get_intrinsic() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                 // class get class <
      lua_remove(the_lua_state, -3);                    // get class <
      lua_pushstring(the_lua_state, name.c_str());      // get class name <

      int num_results = lua_stack_slots<ReturnType>();
      int result = lua_pcall(the_lua_state, 2, num_results, 0);   // (result|nil|err) <
      if (result == 0)
      {
        return_value = pop_value_from_lua<ReturnType>();
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get_intrinsic(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

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
  int start_stack = lua_gettop(the_lua_state);

  // Push type of class onto the stack. (+1)
  lua_getglobal(the_lua_state, type.c_str());           // class <

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    MAJOR_ERROR("Lua class %s was not found", type.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, "set_intrinsic");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:set_intrinsic() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                         // class set class <
      lua_remove(the_lua_state, -3);                            // set class <
      lua_pushstring(the_lua_state, name.c_str());              // set class name <
      push_value_to_lua<ValueType>(value);                      // set class name value <
      int num_args = 2 + lua_stack_slots<ValueType>();
      int result = lua_pcall(the_lua_state, num_args, 0, 0);    // (nil|err) <
      if (result != 0)
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:set_intrinsic(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:set_intrinsic): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }
}

#endif // LUACALLS_H
