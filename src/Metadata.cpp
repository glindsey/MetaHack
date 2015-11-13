#include "Metadata.h"

#include "ErrorHandler.h"
#include "MetadataCollection.h"

Metadata::Metadata(MetadataCollection& collection, std::string type)
  :
  m_collection{ collection },
  m_type{ type }
{
  std::string category = collection.get_category();

  TRACE("Loading metadata for %s!%s...", category.c_str(), type.c_str());

  // Look for the various files containing this metadata.
  std::string resource_string = "resources/" + category + "s/" + type;
  std::string xmlfile_string = resource_string + ".xml";
  fs::path xmlfile_path = fs::path(xmlfile_string);
  std::string pngfile_string = resource_string + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);
  std::string luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  if (!fs::exists(xmlfile_path))
  {
    FATAL_ERROR("Can't find %s", xmlfile_string.c_str());
  }

  /// Load file.
  pt::xml_parser::read_xml(xmlfile_string, m_raw_ptree);

  // DEBUG: Dump the tree using trace.
  //this->trace_tree();

  pt::ptree& data = (m_raw_ptree).get_child(category);

  // Get thing's parent.
  if (data.count("parent") == 0)
  {
    m_parent = "";
  }
  else
  {
    m_parent = data.get_child("parent").get_value<std::string>("");
    strip_quotes(m_parent);
    m_collection.get(m_parent).m_children.push_back(m_type);
  }

  // Get the pretty name.
  if (data.count("name") == 0)
  {
    m_display_name = "[" + type + "]";
  }
  else
  {
    m_display_name = data.get_child("name").get_value<std::string>("[" + type + "]");
    boost::trim(m_display_name);
    strip_quotes(m_display_name);
  }

  // Get thing's pretty plural, if present. Otherwise add "s" to the normal pretty name.
  if (data.count("plural") == 0)
  {
    m_display_plural = m_display_name + "s";
  }
  else
  {
    m_display_plural = data.get_child("plural").get_value<std::string>(m_display_name + "s");
    boost::trim(m_display_plural);
    strip_quotes(m_display_plural);
  }

  // Get thing's description.
  if (data.count("description") == 0)
  {
    m_description = "(No description found.)";
  }
  else
  {
    m_description = data.get_child("description").get_value<std::string>("(No description found.)");
    boost::trim(m_description);
    strip_quotes(m_description);
  }

  // Look for intrinsics, defaults sections. Both must be present in any metadata file.
  pt::ptree intrinsics_tree;
  if (data.count("intrinsics") == 0)
  {
    FATAL_ERROR("Metadata file for %s!%s doesn't have an \"intrinsics\" section", category.c_str(), m_type.c_str());
  }
  else
  {
    intrinsics_tree = data.get_child("intrinsics");
  }

  pt::ptree defaults_tree;
  if (data.count("defaults") == 0)
  {
    FATAL_ERROR("Metadata file for %s!%s doesn't have a \"defaults\" section", category.c_str(), m_type.c_str());
  }
  else
  {
    defaults_tree = data.get_child("defaults");
  }

  // Populate intrinsics dictionary.
  m_intrinsics.populate_from(intrinsics_tree);

  // Populate defaults dictionary.
  m_defaults.populate_from(defaults_tree);

  if (fs::exists(pngfile_path))
  {
    m_has_tiles = true;
    m_tile_location = TS.load_collection(pngfile_string);
    TRACE("Tiles for %s!%s were placed on the TileSheet at (%u, %u)",
      category.c_str(), type.c_str(), m_tile_location.x, m_tile_location.y);
  }
  else
  {
    TRACE("No tiles found for %s!%s", category.c_str(), type.c_str());
    m_has_tiles = false;
  }

  /// Now try to load and run a Lua script for this Thing if one exists.
  if (fs::exists(luafile_path))
  {
    TRACE("Loading Lua script for %s!%s...", category.c_str(), type.c_str());

    the_lua_instance.require(resource_string, true);
  }
}

Metadata::~Metadata()
{}

MetadataCollection& Metadata::get_collection()
{
  return m_collection;
}

std::string const& Metadata::get_type() const
{
  return m_type;
}

std::string const& Metadata::get_parent() const
{
  return m_parent;
}

std::string const& Metadata::get_display_name() const
{
  return m_display_name;
}

std::string const& Metadata::get_display_plural() const
{
  return m_display_plural;
}

std::string const& Metadata::get_description() const
{
  return m_description;
}

sf::Vector2u const& Metadata::get_tile_coords() const
{
  return m_tile_location;
}

// === PROTECTED METHODS ======================================================

/// Get the entire PropertyDictionary of defaults.
PropertyDictionary const& Metadata::get_defaults() const
{
  return m_defaults;
}

/// Get the entire PropertyDictionary of intrinsics.
PropertyDictionary const& Metadata::get_intrinsics() const
{
  return m_intrinsics;
}

/// Recursive function that iterates through the tree and prints the values.
void Metadata::trace_tree(pt::ptree const* pTree = nullptr, std::string prefix = "")
{
  if (pTree == nullptr)
  {
    pTree = &(m_raw_ptree);
  }

  pt::ptree::const_iterator end = pTree->end();
  for (pt::ptree::const_iterator it = pTree->begin(); it != end; ++it)
  {
    std::string key = prefix + (it->first);
    boost::to_lower(key);
    std::string value = it->second.get_value<std::string>();
    boost::trim(key);
    boost::trim(value);
    if (!value.empty())
    {
      TRACE("%s = \"%s\"", key.c_str(), value.c_str());
    }
    trace_tree(&(it->second), key + ".");
  }
}

/// Get the raw property tree containing metadata.
pt::ptree const& Metadata::get_ptree()
{
  std::string category = get_collection().get_category();
  return m_raw_ptree.get_child(category);
}

/// Clear the raw property tree. Should only be done after all data needed
/// has been read from it.
void Metadata::clear_ptree()
{
  m_raw_ptree.clear();
}

ActionResult Metadata::call_lua_function(std::string function_name,
  ThingRef caller,
  std::vector<lua_Integer> const& args,
  ActionResult default_result)
{
  ActionResult return_value = default_result;
  std::string name = this->get_type();

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
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

bool Metadata::call_lua_function_bool(std::string function_name,
  ThingRef caller,
  std::vector<lua_Integer> const& args,
  bool default_result)
{
  bool return_value = default_result;
  std::string name = this->get_type();

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
      for (auto& arg : args)
      {
        lua_pushinteger(the_lua_state, arg);
      }

      // Call the function with N+1 arguments and 1 result. (-(N+2), +1) = -N
      int result = lua_pcall(the_lua_state, args.size() + 1, 1, 0);
      if (result == 0)
      {
        // Get the return value.
        return_value = (lua_toboolean(the_lua_state, -1) != 0);

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
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

sf::Vector2u Metadata::call_lua_function_v2u(std::string function_name,
  ThingRef caller,
  std::vector<lua_Integer> const& args,
  sf::Vector2u default_result)
{
  sf::Vector2u return_value = default_result;
  std::string name = this->get_type();

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
      for (auto& arg : args)
      {
        lua_pushinteger(the_lua_state, arg);
      }

      // Call the function with N+1 arguments and 2 results. (-(N+2), +2) = -N
      int result = lua_pcall(the_lua_state, args.size() + 1, 2, 0);
      if (result == 0)
      {
        // Get the return values.
        return_value = sf::Vector2u(lua_tointeger(the_lua_state, -2), lua_tointeger(the_lua_state, -1));

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
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

bool Metadata::get_lua_bool(std::string name, bool default_value)
{
  bool return_value = default_value;
  std::string type = this->get_type();

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
    lua_getfield(the_lua_state, -1, "get");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                 // class get class <
      lua_remove(the_lua_state, -3);                    // get class <
      lua_pushstring(the_lua_state, name.c_str());      // get class name <
      int result = lua_pcall(the_lua_state, 2, 1, 0);   // (result|err) <
      if (result == 0)
      {
        // Get the return value.
        return_value = (lua_toboolean(the_lua_state, -1) != 0 ? true : false);

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.get): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

double Metadata::get_lua_value(std::string name, double default_value)
{
  double return_value = default_value;
  std::string type = this->get_type();

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
    lua_getfield(the_lua_state, -1, "get");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                 // class get class <
      lua_remove(the_lua_state, -3);                    // get class <
      lua_pushstring(the_lua_state, name.c_str());      // get class name <
      int result = lua_pcall(the_lua_state, 2, 1, 0);   // (result|err) <
      if (result == 0)
      {
        // Get the return value.
        return_value = lua_tonumber(the_lua_state, -1);

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.get): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

std::string Metadata::get_lua_string(std::string name, std::string default_value)
{
  std::string return_value = default_value;
  std::string type = this->get_type();

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
    lua_getfield(the_lua_state, -1, "get");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                 // class get class <
      lua_remove(the_lua_state, -3);                    // get class <
      lua_pushstring(the_lua_state, name.c_str());      // get class intrinsic_name <
      int result = lua_pcall(the_lua_state, 2, 1, 0);   // (result|err) <
      if (result == 0)
      {
        // Get the return value.
        return_value = lua_tostring(the_lua_state, -1);

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.get): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

void Metadata::set_lua_bool(std::string name, bool value)
{
  std::string type = this->get_type();

  int start_stack = lua_gettop(the_lua_state);

  // Push type of class onto the stack. (+1)
  lua_getglobal(the_lua_state, type.c_str());                   // class <

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    MAJOR_ERROR("Lua class %s was not found", type.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, "get");                     // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get() was not found", type.c_str());
    }
    else
    {
      lua_pushvalue(the_lua_state, -2);                         // class get class <
      lua_remove(the_lua_state, -3);                            // get class <
      lua_pushstring(the_lua_state, name.c_str());              // get class name <
      lua_pushboolean(the_lua_state, static_cast<int>(value));  // get class name value <
      int result = lua_pcall(the_lua_state, 3, 0, 0);           // (nil|err) <
      if (result == 0)
      {
        return;
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.get): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }
}

void Metadata::set_lua_value(std::string name, double value)
{
  std::string type = this->get_type();

  int start_stack = lua_gettop(the_lua_state);

  // Push type of class onto the stack. (+1)
  lua_getglobal(the_lua_state, type.c_str());                   // class <

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    MAJOR_ERROR("Lua class %s was not found", type.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, "get");                     // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get() was not found", type.c_str());
    }
    else
    {
      lua_pushvalue(the_lua_state, -2);                         // class get class <
      lua_remove(the_lua_state, -3);                            // get class <
      lua_pushstring(the_lua_state, name.c_str());              // get class name <
      lua_pushnumber(the_lua_state, value);                     // get class name value <
      int result = lua_pcall(the_lua_state, 3, 0, 0);           // (nil|err) <
      if (result == 0)
      {
        return;
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.get): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }
}

void Metadata::set_lua_string(std::string name, std::string value)
{
  std::string type = this->get_type();

  int start_stack = lua_gettop(the_lua_state);

  // Push type of class onto the stack. (+1)
  lua_getglobal(the_lua_state, type.c_str());                   // class <

  if (lua_isnoneornil(the_lua_state, -1))
  {
    // Class not found -- pop the name back off. (-1)
    lua_pop(the_lua_state, 1);
    MAJOR_ERROR("Lua class %s was not found", type.c_str());
  }
  else
  {
    // Push name of function onto the stack. (+1)
    lua_getfield(the_lua_state, -1, "get");                     // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get() was not found", type.c_str());
    }
    else
    {
      lua_pushvalue(the_lua_state, -2);                         // class get class <
      lua_remove(the_lua_state, -3);                            // get class <
      lua_pushstring(the_lua_state, name.c_str());              // get class name <
      lua_pushstring(the_lua_state, value.c_str());             // get class name value <
      int result = lua_pcall(the_lua_state, 3, 0, 0);           // (nil|err) <
      if (result == 0)
      {
        return;
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    MAJOR_ERROR("*** LUA STACK MISMATCH (%s.get): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }
}
