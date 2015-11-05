#include "ThingMetadata.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <SFML/Graphics.hpp>

#include "App.h"
#include "Direction.h"
#include "ErrorHandler.h"
#include "Exceptions.h"
#include "Lua.h"
#include "ThingManager.h"
#include "TileSheet.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Static declarations
boost::ptr_unordered_map<std::string, ThingMetadata> ThingMetadata::s_collection;

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

ThingMetadata::ThingMetadata(std::string type)
  : Metadata("thing", type)
{}


ThingMetadata::~ThingMetadata()
{}

ThingMetadata& ThingMetadata::get(std::string type)
{
  if (type.empty())
  {
    type = "Unknown";
  }

  if (s_collection.count(type) == 0)
  {
    s_collection.insert(type, NEW ThingMetadata(type));
  }

  return s_collection.at(type);
}

ActionResult ThingMetadata::call_lua_function(std::string function_name,
  ThingRef caller,
  std::vector<lua_Integer> const& args,
  ActionResult default_result)
{
  ActionResult return_value = default_result;
  bool call_parent = false;
  std::string name = this->get_name();

  int start_stack = lua_gettop(the_lua_state);

  // Push name of class onto the stack. (+1)
  lua_getglobal(the_lua_state, name.c_str());        

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);                     
    call_parent = true;
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, function_name.c_str());

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      call_parent = true;
    }
    else
    {
      // Remove the class name from the stack. (-1)
      lua_remove(the_lua_state, -2);

      // Push the caller's ID onto the stack. (+1)
      lua_pushinteger(the_lua_state, caller);

      // Push the arguments onto the stack. (+N)
      for (auto& arg : args)
      {
        lua_pushinteger(the_lua_state, arg);
      }

      // Call the function with N+1 arguments and 1 result. (-(N+2), +1) = -(N+1)
      int result = lua_pcall(the_lua_state, args.size() + 1, 1, 0);
      if (result == 0)
      {
        // Get the return value.
        return_value = static_cast<ActionResult>(the_lua_instance.get_enum_value(-1));

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s.%s: %s", name.c_str(), function_name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }

      // Check if the return value is ActionResult::Pending.
      if (return_value == ActionResult::Pending)
      {
        call_parent = true;
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s:%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  std::string parent = get_parent();
  if (call_parent)
  {
    if (parent.empty())
    {
      TRACE("Reached the top of the parent tree trying to call %s.%s", name.c_str(), function_name.c_str());
      return_value = default_result;
    }
    else
    {
      return_value =
        ThingMetadata::get(parent).call_lua_function(function_name, caller, args, default_result);
    }
  }

  return return_value;
}

bool ThingMetadata::call_lua_function_bool(std::string function_name,
  ThingRef caller,
  std::vector<lua_Integer> const& args,
  bool default_result)
{
  bool return_value = default_result;
  bool call_parent = false;
  std::string name = this->get_name();

  int start_stack = lua_gettop(the_lua_state);

  // Push name of class onto the stack. (+1)
  lua_getglobal(the_lua_state, name.c_str());

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    call_parent = true;
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, function_name.c_str());

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      call_parent = true;
    }
    else
    {
      // Remove the class name from the stack. (-1)
      lua_remove(the_lua_state, -2);

      // Push the caller's ID onto the stack. (+1)
      lua_pushinteger(the_lua_state, caller);

      // Push the arguments onto the stack. (+N)
      for (auto& arg : args)
      {
        lua_pushinteger(the_lua_state, arg);
      }

      // Call the function with N+1 arguments and 1 result. (-(N+2), +1) = -N
      int result = lua_pcall(the_lua_state, args.size() + 1, 1, 0);
      if (result == 0)
      {
        // Check for nil return.
        if (lua_isnoneornil(the_lua_state, -1))
        {
          call_parent = true;
        }
        else
        {
          // Get the return value.
          return_value = (lua_toboolean(the_lua_state, -1) != 0);
        }

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
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
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s:%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  if (call_parent)
  {
    std::string parent = get_parent();
    if (parent.empty())
    {
      TRACE("Reached the top of the parent tree trying to call %s.%s", name.c_str(), function_name.c_str());
      return_value = default_result;
    }
    else
    {
      return_value =
        ThingMetadata::get(parent).call_lua_function_bool(function_name, caller, args, default_result);
    }
  }

  return return_value;
}

sf::Vector2u ThingMetadata::call_lua_function_v2u(std::string function_name,
  ThingRef caller,
  std::vector<lua_Integer> const& args,
  sf::Vector2u default_result)
{
  sf::Vector2u return_value = default_result;
  bool call_parent = false;
  std::string name = this->get_name();

  int start_stack = lua_gettop(the_lua_state);

  // Push name of class onto the stack. (+1)
  lua_getglobal(the_lua_state, name.c_str());

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    call_parent = true;
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, function_name.c_str());

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      call_parent = true;
    }
    else
    {
      // Remove the class name from the stack. (-1)
      lua_remove(the_lua_state, -2);

      // Push the caller's ID onto the stack. (+1)
      lua_pushinteger(the_lua_state, caller);

      // Push the arguments onto the stack. (+N)
      for (auto& arg : args)
      {
        lua_pushinteger(the_lua_state, arg);
      }

      // Call the function with N+1 arguments and 2 results. (-(N+2), +2) = -N
      int result = lua_pcall(the_lua_state, args.size() + 1, 2, 0);
      if (result == 0)
      {
        // Check for nil return.
        if (lua_isnoneornil(the_lua_state, -1))
        {
          call_parent = true;
        }
        else
        {
          // Get the return values.
          return_value = sf::Vector2u(lua_tointeger(the_lua_state, -2), lua_tointeger(the_lua_state, -1));
        }

        // Pop the return values off the stack. (-2)
        lua_pop(the_lua_state, 2);
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
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s:%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  if (call_parent)
  {
    std::string parent = get_parent();
    if (parent.empty())
    {
      TRACE("Reached the top of the parent tree trying to call %s.%s", name.c_str(), function_name.c_str());
      return_value = default_result;
    }
    else
    {
      return_value =
        ThingMetadata::get(parent).call_lua_function_v2u(function_name, caller, args, default_result);
    }
  }

  return return_value;
}
