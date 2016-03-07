#include "stdafx.h"

#include "Metadata.h"

#include "ErrorHandler.h"
#include "MetadataCollection.h"

Metadata::Metadata(MetadataCollection& collection, std::string type)
  :
  m_collection{ collection },
  m_type{ type }
{
  SET_UP_LOGGER("Metadata", true);

  std::string category = collection.get_category();

  // Look for the various files containing this metadata.
  std::string resource_string = "resources/" + category + "s/" + type;
  std::string pngfile_string = resource_string + ".png";
  fs::path pngfile_path = fs::path(pngfile_string);
  std::string luafile_string = resource_string + ".lua";
  fs::path luafile_path = fs::path(luafile_string);

  std::string qualified_name = category + "!" + type;

  /// Try to load and run this Thing's Lua script.
  if (fs::exists(luafile_path))
  {
    CLOG(INFO, "Metadata") << "Loading Lua script for " <<
      qualified_name;

    the_lua_instance.require(resource_string, true);
  }
  else
  {
    LOG(FATAL) << "Can't find " << luafile_string;
  }

  if (fs::exists(pngfile_path))
  {
    sf::Vector2u tile_location;

    tile_location = TS.load_collection(pngfile_string);
    CLOG(TRACE, "Metadata") << "Tiles for " << qualified_name <<
      " were placed on the TileSheet at " << tile_location;

    set_intrinsic<bool>("has_tiles", true);
    set_intrinsic<int>("tile_location_x", tile_location.x);
    set_intrinsic<int>("tile_location_y", tile_location.y);
  }
  else
  {
    CLOG(TRACE, "Metadata") << "No tiles found for " << qualified_name;
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

sf::Vector2u Metadata::get_tile_coords()
{
  sf::Vector2u tile_location;
  tile_location.x = get_intrinsic<int>("tile_location_x");
  tile_location.y = get_intrinsic<int>("tile_location_y");
  return tile_location;
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
        if (!lua_istable(the_lua_state, -1))
        {
          FATAL_ERROR("Expected ActionResult from Lua function but got %s", lua_typename(the_lua_state, lua_type(the_lua_state, -1)));
        }

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
    FATAL_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
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
        if (!lua_isboolean(the_lua_state, -1))
        {
          FATAL_ERROR("Expected boolean from Lua function but got %s", lua_typename(the_lua_state, lua_type(the_lua_state, -1)));
        }

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
    FATAL_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
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
        if (!lua_isnumber(the_lua_state, -2) || !lua_isnumber(the_lua_state, -1))
        {
          FATAL_ERROR("Expected number/number from Lua function but got %s/%s",
                      lua_typename(the_lua_state, lua_type(the_lua_state, -2)),
                      lua_typename(the_lua_state, lua_type(the_lua_state, -1)));
        }

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
    FATAL_ERROR("*** LUA STACK MISMATCH (%s.%s): Started at %d, ended at %d", name.c_str(), function_name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

bool Metadata::get_default_bool(std::string name, bool default_value = false)
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
    lua_getfield(the_lua_state, -1, "get_default");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get_default() was not found", type.c_str());
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
        if (!lua_isnoneornil(the_lua_state, -1))
        {
          // Get the return value.
          return_value = (lua_toboolean(the_lua_state, -1) != 0 ? true : false);
        }

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get_default(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:get_default): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

double Metadata::get_default_value(std::string name, double default_value = 0.0)
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
    lua_getfield(the_lua_state, -1, "get_default");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get_default() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                 // class get class <
      lua_remove(the_lua_state, -3);                    // get class <
      lua_pushstring(the_lua_state, name.c_str());      // get class name <
      int result = lua_pcall(the_lua_state, 2, 2, 0);   // (result1, (result2|nil)|err) <
      if (result == 0)
      {
        // Get the return value.
        if (!lua_isnoneornil(the_lua_state, -2))
        {
          if (lua_isnoneornil(the_lua_state, -1))
          {
            // Get the return value.
            return_value = lua_tonumber(the_lua_state, -2);
          }
          else
          {
            IntegerRange range(static_cast<int>(lua_tonumber(the_lua_state, -2)), static_cast<int>(lua_tonumber(the_lua_state, -1)));
            return_value = range.pick();
          }
        }

        // Pop the return values off the stack. (-2)
        lua_pop(the_lua_state, 2);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get_default(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:get_default): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

std::string Metadata::get_default_string(std::string name, std::string default_value = "")
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
    lua_getfield(the_lua_state, -1, "get_default");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get_default() was not found", type.c_str());
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
        if (!lua_isnoneornil(the_lua_state, -1))
        {
          return_value = lua_tostring(the_lua_state, -1);
        }

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get_default(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:get_default): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

IntegerRange Metadata::get_default_range(std::string name, IntegerRange default_value = IntegerRange(0, 0))
{
  IntegerRange return_value = default_value;
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
    lua_getfield(the_lua_state, -1, "get_default");             // class get <

    if (lua_isnoneornil(the_lua_state, -1))
    {
      // Function not found -- pop the function and class names back off. (-2)
      lua_pop(the_lua_state, 2);
      MAJOR_ERROR("Lua function %s:get_default() was not found", type.c_str());
    }
    else
    {
      // Push the class name up to the front of the stack.
      lua_pushvalue(the_lua_state, -2);                 // class get class <
      lua_remove(the_lua_state, -3);                    // get class <
      lua_pushstring(the_lua_state, name.c_str());      // get class intrinsic_name <
      int result = lua_pcall(the_lua_state, 2, 2, 0);   // (result1,result2|err) <
      if (result == 0)
      {
        // Get the return value.
        if (!lua_isnoneornil(the_lua_state, -1) && !lua_isnoneornil(the_lua_state, -2))
        {
          return_value = IntegerRange(static_cast<int>(lua_tonumber(the_lua_state, -2)), static_cast<int>(lua_tonumber(the_lua_state, -1)));
        }

        // Pop the return values off the stack. (-2)
        lua_pop(the_lua_state, 2);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        MAJOR_ERROR("Error calling %s:get_default(%s): %s", type.c_str(), name.c_str(), error_message);

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    FATAL_ERROR("*** LUA STACK MISMATCH (%s:get_default): Started at %d, ended at %d", name.c_str(), start_stack, end_stack);
  }

  return return_value;
}

bool Metadata::get_intrinsic_bool(std::string name, bool default_value = false)
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
      int result = lua_pcall(the_lua_state, 2, 1, 0);   // (result|err) <
      if (result == 0)
      {
        // Get the return value.
        if (!lua_isnoneornil(the_lua_state, -1))
        {
          // Get the return value.
          return_value = (lua_toboolean(the_lua_state, -1) != 0 ? true : false);
        }

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
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

double Metadata::get_intrinsic_value(std::string name, double default_value = 0.0)
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
      int result = lua_pcall(the_lua_state, 2, 1, 0);   // (result|err) <
      if (result == 0)
      {
        // Get the return value.
        if (!lua_isnoneornil(the_lua_state, -1))
        {
          // Get the return value.
          return_value = lua_tonumber(the_lua_state, -1);
        }

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
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

std::string Metadata::get_intrinsic_string(std::string name, std::string default_value = "")
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
      lua_pushstring(the_lua_state, name.c_str());      // get class intrinsic_name <
      int result = lua_pcall(the_lua_state, 2, 1, 0);   // (result|err) <
      if (result == 0)
      {
        // Get the return value.
        if (!lua_isnoneornil(the_lua_state, -1))
        {
          return_value = lua_tostring(the_lua_state, -1);
        }
        else
        {
          CLOG(WARNING, "Metadata") << "Warning calling " << type << ":get_intrinsic(" << name << "): Result was nil";
        }

        // Pop the return value off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
      else
      {
        // Get the error message.
        char const* error_message = lua_tostring(the_lua_state, -1);
        CLOG(WARNING, "Metadata") << "Error calling " << type << ":get_intrinsic(" << name << "): " << error_message;

        // Pop the error message off the stack. (-1)
        lua_pop(the_lua_state, 1);
      }
    }
  }

  int end_stack = lua_gettop(the_lua_state);

  if (start_stack != end_stack)
  {
    CLOG(FATAL, "Metadata") << "*** LUA STACK MISMATCH (" << name <<
      ":get_intrinsic): Started at " << start_stack << ", ended at " << end_stack;
  }

  return return_value;
}

void Metadata::set_intrinsic_bool(std::string name, bool value)
{
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
      lua_pushboolean(the_lua_state, static_cast<int>(value));  // set class name value <
      int result = lua_pcall(the_lua_state, 3, 0, 0);           // (nil|err) <
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

void Metadata::set_intrinsic_value(std::string name, double value = 0.0)
{
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
    lua_getfield(the_lua_state, -1, "set_intrinsic");             // class set <

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
      lua_pushnumber(the_lua_state, value);                     // set class name value <
      int result = lua_pcall(the_lua_state, 3, 0, 0);           // (nil|err) <
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

void Metadata::set_intrinsic_string(std::string name, std::string value = "")
{
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
    lua_getfield(the_lua_state, -1, "set_intrinsic");             // class set <

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
      lua_pushstring(the_lua_state, value.c_str());             // set class name value <
      int result = lua_pcall(the_lua_state, 3, 0, 0);           // (nil|err) <
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