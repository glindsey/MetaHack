#include "ThingMetadata.h"

#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <SFML/Graphics.hpp>

#include "ErrorHandler.h"
#include "Exceptions.h"
#include "Lua.h"
#include "ThingManager.h"
#include "TileSheet.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Namespace aliases
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

// Using declarations

struct ThingMetadata::Impl
{
  /// Thing's own name.
  std::string name;

  /// Thing pretty name.
  std::string pretty_name;

  /// Thing pretty plural.
  std::string pretty_plural;

  /// Thing parent type, if any.
  std::string parent;

  /// Thing description.
  std::string description;

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

  /// Boolean indicating whether this Thing has graphics associated with it.
  bool has_tiles;

  /// Location of this Thing's graphics on the tilesheet.
  sf::Vector2u tile_location;
};


ThingMetadata::ThingMetadata(std::string type)
  : pImpl(new Impl())
{
  TRACE("Loading metadata for type \"%s\"...", type.c_str());

  pImpl->name = type;

  // Look for the XML/PNG files for this thing.
  std::string xmlfile_string = "resources/things/" + type + ".xml";
  fs::path xmlfile_path = fs::path(xmlfile_string);
  std::string pngfile_string = "resources/things/" + type + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);
  std::string luafile_string = "resources/things/" + type + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  if (!fs::exists(xmlfile_path))
  {
    throw ExceptionMissingFile("XML", "Thing", type);
  }

  //TRACE("Found file \"%s\"", xmlfile_string.c_str());
    
  /// @todo Load file.
  pt::ptree data;
  pt::xml_parser::read_xml(xmlfile_string, data);

  //TRACE("Loaded property tree for \"%s\"", type.c_str());

  // Get thing's pretty name.
  try
  { 
    pImpl->pretty_name = data.get_child("thing.name").get_value<std::string>("[" + type + "]");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->pretty_name = "[" + type + "]";
  }

  // Get thing's pretty plural, if present. Otherwise add "s" to the normal pretty name.
  try
  {
    pImpl->pretty_plural = data.get_child("thing.plural").get_value<std::string>(pImpl->pretty_name + "s");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->pretty_plural = pImpl->pretty_name + "s";
  }

  // Get thing's parent.
  try
  {
    pImpl->parent = data.get_child("thing.parent").get_value<std::string>("");
  }
  catch (pt::ptree_bad_path&)
  {
    if (type != "Mu" && type != "Thing")
    {
      MINOR_ERROR("\"%s\" has no specified parent; this is probably not intentional", type.c_str());
    }

    pImpl->parent = "";
  }

  // Get thing's description.
  try
  {
    pImpl->description = data.get_child("thing.description").get_value<std::string>("(No description found.)");
  }
  catch (pt::ptree_bad_path&)
  {
    pImpl->description = "(No description found.)";
  }

  // Look for intrinsics, properties sections. Both must be present for any Thing.
  pt::ptree intrinsics_tree = data.get_child("thing.intrinsics");
  pt::ptree properties_tree = data.get_child("thing.properties");
  try
  {
    intrinsics_tree = data.get_child("thing.intrinsics");
    properties_tree = data.get_child("thing.properties");
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
      //TRACE("Found intrinsic flag: %s = %s", key.c_str(), (value == true) ? "true" : "false");
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

  if (fs::exists(pngfile_path))
  {
    pImpl->has_tiles = true;
    pImpl->tile_location = TS.load_collection(pngfile_string);
    TRACE("Tiles for Thing %s were placed on the TileSheet at (%u, %u)", 
      type.c_str(), pImpl->tile_location.x, pImpl->tile_location.y);
  }
  else
  {
    pImpl->has_tiles = false;
  }

  /// Now try to load and run a Lua script for this Thing if one exists.
  if (fs::exists(luafile_path))
  {
    TRACE("Loading Lua script for type \"%s\"...", type.c_str());
    the_lua_instance.do_file(luafile_string);
  }

}


ThingMetadata::~ThingMetadata()
{
}

std::string const& ThingMetadata::get_pretty_name() const
{
  return pImpl->pretty_name;
}

std::string const& ThingMetadata::get_pretty_plural() const
{
  return pImpl->pretty_plural;
}

std::string const& ThingMetadata::get_description() const
{
  return pImpl->description;
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
      return TM.get_metadata(pImpl->parent).get_intrinsic_flag(key, default_value);
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
      return TM.get_metadata(pImpl->parent).get_intrinsic_value(key, default_value);
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
      return TM.get_metadata(pImpl->parent).get_intrinsic_string(key, default_value);
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
      return TM.get_metadata(pImpl->parent).get_default_flag(key, default_value);
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
      return TM.get_metadata(pImpl->parent).get_default_value(key, default_value);
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
      return TM.get_metadata(pImpl->parent).get_default_string(key, default_value);
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

sf::Vector2u ThingMetadata::get_tile_coords() const
{
  return pImpl->tile_location;
}

ActionResult ThingMetadata::call_lua_function_1(std::string function_name,
                                                ThingId caller,
                                                ActionResult default_result)
{
  ActionResult return_value = default_result;

  std::string name = pImpl->name;
  lua_getglobal(the_lua_state, name.c_str());        // <1 Push name of class

  if (!lua_isnoneornil(the_lua_state, -1))
  {
    lua_getfield(the_lua_state, -1, function_name.c_str()); // <2  Push the function name
    lua_remove(the_lua_state, -2);                          // >1  Get rid of name of class
    lua_pushinteger(the_lua_state, caller.full_id);         // <2  Push the thing's ID
    int result = lua_pcall(the_lua_state, 1, 1, 0);         // >><1 Call with one argument, one result
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
  }
  else // didn't find a function of that name here, so try parent...
  {
    lua_pop(the_lua_state, 1);                      // >0 Pop the name back off the stack

    if (pImpl->parent.empty())
    {
      return_value = default_result;
    }
    else
    { 
      return_value = TM.get_metadata(pImpl->parent).call_lua_function_1(function_name, 
                                                                        caller,
                                                                        default_result);
    }
  }

  return return_value;
}

ActionResult ThingMetadata::call_lua_function_2(std::string function_name,
                                                ThingId caller,
                                                ThingId target,
                                                ActionResult default_result)
{
  ActionResult return_value = default_result;

  std::string name = pImpl->name;
  lua_getglobal(the_lua_state, name.c_str());        // <1 Push name of class

  if (!lua_isnoneornil(the_lua_state, -1))
  {
    lua_getfield(the_lua_state, -1, function_name.c_str()); // <2  Push the function name
    lua_remove(the_lua_state, -2);                          // >1  Get rid of name of class
    lua_pushinteger(the_lua_state, caller.full_id);         // <2  Push the caller's ID
    lua_pushinteger(the_lua_state, target.full_id);         // <3  Push the target's ID
    int result = lua_pcall(the_lua_state, 2, 1, 0);         // >>><1 Call with two arguments, one result
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
  }
  else // didn't find a function of that name here, so try parent...
  {
    lua_pop(the_lua_state, 1);                      // >0 Pop the name back off the stack

    if (pImpl->parent.empty())
    {
      return_value = default_result;
    }
    else
    {
      return_value = TM.get_metadata(pImpl->parent).call_lua_function_2(function_name,
                                                                        caller,
                                                                        target,
                                                                        default_result);
    }
  }

  return return_value;
}

ActionResult ThingMetadata::call_lua_function_3(std::string function_name,
                                                ThingId caller,
                                                ThingId target1,
                                                ThingId target2,
                                                ActionResult default_result)
{
  ActionResult return_value = default_result;

  std::string name = pImpl->name;
  lua_getglobal(the_lua_state, name.c_str());        // <1 Push name of class

  if (!lua_isnoneornil(the_lua_state, -1))
  {
    lua_getfield(the_lua_state, -1, function_name.c_str()); // <2  Push the function name
    lua_remove(the_lua_state, -2);                          // >1  Get rid of name of class
    lua_pushinteger(the_lua_state, caller.full_id);         // <2  Push the caller's ID
    lua_pushinteger(the_lua_state, target1.full_id);        // <3  Push target #1's ID
    lua_pushinteger(the_lua_state, target2.full_id);        // <3  Push target #2's ID
    int result = lua_pcall(the_lua_state, 3, 1, 0);         // >>><1 Call with three arguments, one result
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
  }
  else // didn't find a function of that name here, so try parent...
  {
    lua_pop(the_lua_state, 1);                      // >0 Pop the name back off the stack

    if (pImpl->parent.empty())
    {
      return_value = default_result;
    }
    else
    {
      return_value = TM.get_metadata(pImpl->parent).call_lua_function_3(function_name,
        caller,
        target1,
        target2,
        default_result);
    }
  }

  return return_value;
}
