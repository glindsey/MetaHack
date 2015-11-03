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
boost::ptr_unordered_map<std::string, ThingMetadata> ThingMetadata::collection;

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

struct ThingMetadata::Impl
{
  /// Thing parent type, if any.
  std::string parent;

  /// Map of intrinsic flags.
  FlagsMap intrinsic_flags;

  /// Map of intrinsic values.
  ValuesMap intrinsic_values;

  /// Map of intrinsic strings.
  StringsMap intrinsic_strings;

  /// Map of default property flags.
  FlagsMap default_flags;

  /// Map of default property values.
  ValuesMap default_values;

  /// Map of default property strings.
  StringsMap default_strings;
};


ThingMetadata::ThingMetadata(std::string type)
  : Metadata("thing", type), pImpl(NEW Impl())
{
  TRACE("Loading Thing-specific metadata for map tile \"%s\"...", type.c_str());

  pt::ptree const& data = get_ptree();

  // Get thing's parent.
  try
  {
    pImpl->parent = data.get_child("parent").get_value<std::string>("");
  }
  catch (pt::ptree_bad_path&)
  {
    if (type != "Mu" && type != "Thing")
    {
      MINOR_ERROR("\"%s\" has no specified parent; this is probably not intentional", type.c_str());
    }

    pImpl->parent = "";
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
  try
  {
    for (auto& child_tree : intrinsics_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      pImpl->intrinsic_flags[key] = value;
    }
  }
  catch (pt::ptree_bad_path&)
  {
    //MINOR_ERROR("\"%s\" has no intrinsic flags section; assuming empty", type.c_str());
  }

  // Get intrinsic values.
  try
  {
    for (auto& child_tree : intrinsics_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      pImpl->intrinsic_values[key] = value;
      //TRACE("Found intrinsic value: %s = %d", key.c_str(), value);
    }
  }
  catch (pt::ptree_bad_path&)
  {
    //MINOR_ERROR("\"%s\" has no intrinsic values section; assuming empty", type.c_str());
  }

  // Get intrinsic strings.
  try
  {
    pt::ptree intrinsic_strings = intrinsics_tree.get_child("strings");

    for (auto& child_tree : intrinsics_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      pImpl->intrinsic_strings[key] = value;
      //TRACE("Found intrinsic string: %s = \"%s\"", key.c_str(), value.c_str());
    }
  }
  catch (pt::ptree_bad_path&)
  {
    //MINOR_ERROR("\"%s\" has no intrinsic strings section; assuming empty", type.c_str());
  }

  // Get default property flags.
  try
  {
    for (auto& child_tree : properties_tree.get_child("flags"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      bool value = child_tree.second.get_value<bool>(false);

      pImpl->default_flags[key] = value;
      //TRACE("Found default flag: %s = %s", key.c_str(), (value == true) ? "true" : "false");
    }
  }
  catch (pt::ptree_bad_path&)
  {
    //MINOR_ERROR("\"%s\" has no property flags section; assuming empty", type.c_str());
  }

  // Get default property values.
  try
  {
    for (auto& child_tree : properties_tree.get_child("values"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      int value = child_tree.second.get_value<int>(0);

      pImpl->default_values[key] = value;
      //TRACE("Found default value: %s = %d", key.c_str(), value);
    }
  }
  catch (pt::ptree_bad_path&)
  {
    //MINOR_ERROR("\"%s\" has no property values section; assuming empty", type.c_str());
  }

  // Get default property strings.
  try
  {
    for (auto& child_tree : properties_tree.get_child("strings"))
    {
      std::string key = child_tree.first;
      boost::algorithm::to_lower(key);
      std::string value = child_tree.second.get_value<std::string>("");

      pImpl->default_strings[key] = value;
      //TRACE("Found default string: %s = \"%s\"", key.c_str(), value.c_str());
    }
  }
  catch (pt::ptree_bad_path&)
  {
    //MINOR_ERROR("\"%s\" has no property strings section; assuming empty", type.c_str());
  }

  //TRACE("done");

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

  if (collection.count(type) == 0)
  {
    collection.insert(type, NEW ThingMetadata(type));
  }

  return collection.at(type);
}


std::string const& ThingMetadata::get_parent() const
{
  return pImpl->parent;
}

bool ThingMetadata::get_intrinsic_flag(std::string key, bool default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->intrinsic_flags.count(key) != 0)
  {
    return pImpl->intrinsic_flags.at(key);
  }
  else
  {
    if (pImpl->parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(pImpl->parent).get_intrinsic_flag(key, default_value);
    }
  }
}

int ThingMetadata::get_intrinsic_value(std::string key, int default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->intrinsic_values.count(key) != 0)
  {
    return pImpl->intrinsic_values.at(key);
  }
  else
  {
    if (pImpl->parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(pImpl->parent).get_intrinsic_value(key, default_value);
    }
  }
}

std::string ThingMetadata::get_intrinsic_string(std::string key, std::string default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->intrinsic_strings.count(key) != 0)
  {
    return pImpl->intrinsic_strings.at(key);
  }
  else
  {
    if (pImpl->parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(pImpl->parent).get_intrinsic_string(key, default_value);
    }
  }
}

FlagsMap const& ThingMetadata::get_intrinsic_flags() const
{
  return pImpl->intrinsic_flags;
}

ValuesMap const& ThingMetadata::get_intrinsic_values() const
{
  return pImpl->intrinsic_values;
}

StringsMap const& ThingMetadata::get_intrinsic_strings() const
{
  return pImpl->intrinsic_strings;
}

bool ThingMetadata::get_default_flag(std::string key, bool default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->default_flags.count(key) != 0)
  {
    return pImpl->default_flags.at(key);
  }
  else
  {
    if (pImpl->parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(pImpl->parent).get_default_flag(key, default_value);
    }
  }
}

int ThingMetadata::get_default_value(std::string key, int default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->default_values.count(key) != 0)
  {
    return pImpl->default_values.at(key);
  }
  else
  {
    if (pImpl->parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(pImpl->parent).get_default_value(key, default_value);
    }
  }
}

std::string ThingMetadata::get_default_string(std::string key, std::string default_value) const
{
  boost::algorithm::to_lower(key);

  if (pImpl->default_strings.count(key) != 0)
  {
    return pImpl->default_strings.at(key);
  }
  else
  {
    if (pImpl->parent.empty())
    {
      return default_value;
    }
    else
    {
      return ThingMetadata::get(pImpl->parent).get_default_string(key, default_value);
    }
  }
}

FlagsMap const& ThingMetadata::get_default_flags() const
{
  return pImpl->default_flags;
}

ValuesMap const& ThingMetadata::get_default_values() const
{
  return pImpl->default_values;
}

StringsMap const& ThingMetadata::get_default_strings() const
{
  return pImpl->default_strings;
}

ActionResult ThingMetadata::call_lua_function(std::string function_name,
                                              ThingRef caller,
                                              ActionResult default_result)
{
  ActionResult return_value = default_result;

  std::string name = this->get_name();
  lua_getglobal(the_lua_state, name.c_str());        // <1 Push name of class

  if (!lua_isnoneornil(the_lua_state, -1))
  {
    lua_getfield(the_lua_state, -1, function_name.c_str());   // <2  Push the function name
    if (!lua_isnoneornil(the_lua_state, -1))
    {
      lua_remove(the_lua_state, -2);                            // >1  Get rid of name of class
      lua_pushinteger(the_lua_state, caller.get_id().full_id);  // <2  Push the caller's ID
      int result = lua_pcall(the_lua_state, 1, 1, 0);           // >><1 Call with one argument, one result
      if (result == LUA_OK)
      {
        return_value = (ActionResult)lua_tointeger(the_lua_state, -1);
        lua_pop(the_lua_state, 1);                              // >0
      }
      else
      {
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s.%s: %s", name.c_str(), function_name.c_str(), error_message);
        lua_pop(the_lua_state, 1);                              // >0
      }

      // If we got here, return.
      return return_value;
    }
    else // didn't find a function of that name here, so try parent...
    {
      lua_pop(the_lua_state, 1);                      // >1 Pop the function name back off the stack
    }
  }

  lua_pop(the_lua_state, 1);                      // >1 Pop the class name back off the stack

  if (pImpl->parent.empty())
  {
    return_value = default_result;
  }
  else
  {
    return_value = ThingMetadata::get(pImpl->parent).call_lua_function(function_name, caller, default_result);
  }

  return return_value;
}

ActionResult ThingMetadata::call_lua_function(std::string function_name,
                                                ThingRef caller,
                                                lua_Integer arg,
                                                ActionResult default_result)
{
  ActionResult return_value = default_result;

  std::string name = this->get_name();
  lua_getglobal(the_lua_state, name.c_str());        // <1 Push name of class

  if (!lua_isnoneornil(the_lua_state, -1))
  {
    lua_getfield(the_lua_state, -1, function_name.c_str());   // <2  Push the function name
    if (!lua_isnoneornil(the_lua_state, -1))
    {
      lua_remove(the_lua_state, -2);                            // >1  Get rid of name of class
      lua_pushinteger(the_lua_state, caller.get_id().full_id);  // <2  Push the caller's ID
      lua_pushinteger(the_lua_state, arg);                      // <3  Push the argument
      int result = lua_pcall(the_lua_state, 2, 1, 0);           // >>><1 Call with two arguments, one result
      if (result == LUA_OK)
      {
        return_value = (ActionResult)lua_tointeger(the_lua_state, -1);
        lua_pop(the_lua_state, 1);
      }
      else
      {
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s.%s: %s", name.c_str(), function_name.c_str(), error_message);
        lua_pop(the_lua_state, 1);
      }

      // If we got here, return.
      return return_value;
    }
    else // didn't find a function of that name here, so try parent...
    {
      lua_pop(the_lua_state, 1);                      // >1 Pop the function name back off the stack
    }
  }

  lua_pop(the_lua_state, 1);                      // >1 Pop the class name back off the stack

  if (pImpl->parent.empty())
  {
    return_value = default_result;
  }
  else
  {
    return_value = ThingMetadata::get(pImpl->parent).call_lua_function(function_name, caller, default_result);
  }

  return return_value;
}

ActionResult ThingMetadata::call_lua_function(std::string function_name,
                                                ThingRef caller,
                                                lua_Integer arg1,
                                                lua_Integer arg2,
                                                ActionResult default_result)
{
  ActionResult return_value = default_result;

  std::string name = this->get_name();
  lua_getglobal(the_lua_state, name.c_str());        // <1 Push name of class

  if (!lua_isnoneornil(the_lua_state, -1))
  {
    lua_getfield(the_lua_state, -1, function_name.c_str());   // <2  Push the function name
    if (!lua_isnoneornil(the_lua_state, -1))
    {
      lua_remove(the_lua_state, -2);                            // >1  Get rid of name of class
      lua_pushinteger(the_lua_state, caller.get_id().full_id);  // <2  Push the caller's ID
      lua_pushinteger(the_lua_state, arg1);                     // <3  Push the argument #1
      lua_pushinteger(the_lua_state, arg2);                     // <4  Push the argument #2
      int result = lua_pcall(the_lua_state, 3, 1, 0);           // >>>><1 Call with three arguments, one result
      if (result == LUA_OK)
      {
        return_value = (ActionResult)lua_tointeger(the_lua_state, -1);
        lua_pop(the_lua_state, 1);                              // >0 Pop the result off the stack.
      }
      else
      {
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s.%s: %s", name.c_str(), function_name.c_str(), error_message);
        lua_pop(the_lua_state, 1);                              // >0 Pop the error message off the stack.
      }

      // If we got here, and return value is NOT ActionResult::Pending, return.
      if (return_value != ActionResult::Pending)
      {
        return return_value;
      }
    }
    else // didn't find a function of that name here
    {
      lua_pop(the_lua_state, 2);                      // >>0 Pop the function and class names back off the stack
    }
  }
  else // didn't find a class of that name here
  {
    lua_pop(the_lua_state, 1);                      // >0 Pop the class name back off the stack
  }

  if (!pImpl->parent.empty())
  {
    return_value = ThingMetadata::get(pImpl->parent).call_lua_function(function_name, caller, default_result);
  }

  return return_value;
}
