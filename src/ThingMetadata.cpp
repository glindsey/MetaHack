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
{
  TRACE("Loading Thing-specific metadata for map tile \"%s\"...", type.c_str());

  pt::ptree const& data = get_ptree();

  // Get thing's parent.
  try
  {
    m_parent = data.get_child("parent").get_value<std::string>("");
  }
  catch (pt::ptree_bad_path&)
  {
    if (type != "Mu" && type != "Thing")
    {
      MINOR_ERROR("\"%s\" has no specified parent; this is probably not intentional", type.c_str());
    }

    m_parent = "";
  }

  // Look for intrinsics, properties sections. Both must be present for any Thing.
  pt::ptree intrinsics_tree = data.get_child("intrinsics");
  pt::ptree properties_tree = data.get_child("properties");
  try
  {
    intrinsics_tree = data.get_child("intrinsics");
    properties_tree = data.get_child("properties");
  }
  catch (pt::ptree_bad_path& p)
  {
    FATAL_ERROR(p.what());
  }

  // Get intrinsic flags.
  if (intrinsics_tree.count("flags") != 0)
  {
    for (auto& child_tree : intrinsics_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      m_intrinsic_flags[key] = value;
    }
  }

  // Get intrinsic values.
  if (intrinsics_tree.count("values") != 0)
  {
    for (auto& child_tree : intrinsics_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      m_intrinsic_values[key] = value;
    }
  }

  // Get intrinsic strings.
  if (intrinsics_tree.count("strings") != 0)
  {
    for (auto& child_tree : intrinsics_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      m_intrinsic_strings[key] = value;
    }
  }

  // Get default property flags.
  if (properties_tree.count("flags") != 0)
  {
    for (auto& child_tree : properties_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      m_default_flags[key] = value;
    }
  }

  // Get default property values.
  if (properties_tree.count("values") != 0)
  {
    for (auto& child_tree : properties_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      m_default_values[key] = value;
    }
  }

  // Get default property strings.
  if (properties_tree.count("strings") != 0)
  {
    for (auto& child_tree : properties_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      m_default_strings[key] = value;
    }
  }
}


ThingMetadata::~ThingMetadata()
{
}

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


std::string const& ThingMetadata::get_parent() const
{
  return m_parent;
}

bool ThingMetadata::get_intrinsic_flag(std::string key, bool default_value) const
{
  boost::algorithm::to_lower(key);

  if (m_intrinsic_flags.count(key) != 0)
  {
    return m_intrinsic_flags.at(key);
  }
  else
  {
    if (m_parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(m_parent).get_intrinsic_flag(key, default_value);
    }
  }
}

int ThingMetadata::get_intrinsic_value(std::string key, int default_value) const
{
  boost::algorithm::to_lower(key);

  if (m_intrinsic_values.count(key) != 0)
  {
    return m_intrinsic_values.at(key);
  }
  else
  {
    if (m_parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(m_parent).get_intrinsic_value(key, default_value);
    }
  }
}

std::string ThingMetadata::get_intrinsic_string(std::string key, std::string default_value) const
{
  boost::algorithm::to_lower(key);

  if (m_intrinsic_strings.count(key) != 0)
  {
    return m_intrinsic_strings.at(key);
  }
  else
  {
    if (m_parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(m_parent).get_intrinsic_string(key, default_value);
    }
  }
}

FlagsMap const& ThingMetadata::get_intrinsic_flags() const
{
  return m_intrinsic_flags;
}

ValuesMap const& ThingMetadata::get_intrinsic_values() const
{
  return m_intrinsic_values;
}

StringsMap const& ThingMetadata::get_intrinsic_strings() const
{
  return m_intrinsic_strings;
}

bool ThingMetadata::get_default_flag(std::string key, bool default_value) const
{
  boost::algorithm::to_lower(key);

  if (m_default_flags.count(key) != 0)
  {
    return m_default_flags.at(key);
  }
  else
  {
    if (m_parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(m_parent).get_default_flag(key, default_value);
    }
  }
}

int ThingMetadata::get_default_value(std::string key, int default_value) const
{
  boost::algorithm::to_lower(key);

  if (m_default_values.count(key) != 0)
  {
    return m_default_values.at(key);
  }
  else
  {
    if (m_parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(m_parent).get_default_value(key, default_value);
    }
  }
}

std::string ThingMetadata::get_default_string(std::string key, std::string default_value) const
{
  boost::algorithm::to_lower(key);

  if (m_default_strings.count(key) != 0)
  {
    return m_default_strings.at(key);
  }
  else
  {
    if (m_parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(m_parent).get_default_string(key, default_value);
    }
  }
}

FlagsMap const& ThingMetadata::get_default_flags() const
{
  return m_default_flags;
}

ValuesMap const& ThingMetadata::get_default_values() const
{
  return m_default_values;
}

StringsMap const& ThingMetadata::get_default_strings() const
{
  return m_default_strings;
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

  if (call_parent)
  {
    if (m_parent.empty())
    {
      TRACE("Reached the top of the parent tree trying to call %s.%s", name.c_str(), function_name.c_str());
      return_value = default_result;
    }
    else
    {
      return_value =
        ThingMetadata::get(m_parent).call_lua_function(function_name, caller, args, default_result);
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
    if (m_parent.empty())
    {
      TRACE("Reached the top of the parent tree trying to call %s.%s", name.c_str(), function_name.c_str());
      return_value = default_result;
    }
    else
    {
      return_value =
        ThingMetadata::get(m_parent).call_lua_function_bool(function_name, caller, args, default_result);
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
    if (m_parent.empty())
    {
      TRACE("Reached the top of the parent tree trying to call %s.%s", name.c_str(), function_name.c_str());
      return_value = default_result;
    }
    else
    {
      return_value =
        ThingMetadata::get(m_parent).call_lua_function_v2u(function_name, caller, args, default_result);
    }
  }

  return return_value;
}
