#include "stdafx.h"

#include "LuaThingFunctions.h"

#include "ErrorHandler.h"
#include "GameState.h"
#include "Thing.h"
#include "ThingRef.h"

namespace LuaThingFunctions
{
  int thing_create(lua_State* L)
  {
    bool success = false;
    ThingRef new_thing;
    int num_args = lua_gettop(L);

    if ((num_args < 2) || (num_args > 3))
    {
      CLOG(WARNING, "Lua") << "expected 2 or 3 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    std::string new_thing_type = lua_tostring(L, 2);

    // Check to make sure the Thing is actually creatable.
    /// @todo Might want the ability to disable this check for debugging purposes?
    Metadata& thing_metadata = MDC::get_collection("thing").get(new_thing_type);
    bool is_creatable = thing_metadata.get_intrinsic<bool>("creatable", false);

    if (is_creatable)
    {
      new_thing = GAME.get_thing_manager().create(new_thing_type);
      success = new_thing->move_into(thing);

      if (success && (num_args > 2))
      {
        int quantity = lua_tointeger(L, 3);
        new_thing->set_quantity(static_cast<unsigned int>(quantity));
      }
    }

    if (success)
    {
      lua_pushinteger(L, new_thing);
    }
    else
    {
      lua_pushnil(L);
    }

    return 1;
  }

  int thing_destroy(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "Lua") << "expected 1 argument, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    thing->destroy();

    return 0;
  }

  int thing_get_player(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 0)
    {
      CLOG(WARNING, "Lua") << "expected 0 arguments, got " << num_args;
      return 0;
    }

    ThingRef player = GAME.get_player();
    lua_pushinteger(L, player);

    return 1;
  }

  int thing_get_coords(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "Lua") << "expected 1 argument, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));

    MapId map_id = GAME.get_player()->get_map_id();
    auto maptile = thing->get_maptile();

    if (maptile != nullptr)
    {
      auto coords = maptile->get_coords();
      lua_pushinteger(L, coords.x);
      lua_pushinteger(L, coords.y);
    }
    else
    {
      lua_pushnil(L);
      lua_pushnil(L);
    }

    return 2;
  }

  int thing_get_type(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    std::string result = thing->get_type();
    lua_pushstring(L, result.c_str());

    return 1;
  }

  int thing_get_intrinsic_flag(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    bool result = thing->get_intrinsic<bool>(key);
    lua_pushboolean(L, result);

    return 1;
  }

  int thing_get_intrinsic_value(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    int result = thing->get_intrinsic<int>(key);
    lua_pushinteger(L, result);

    return 1;
  }

  int thing_get_intrinsic_string(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);

    std::string result = thing->get_intrinsic<std::string>(key).c_str();

    lua_pushstring(L, result.c_str());

    return 1;
  }

  int thing_get_property_flag(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    bool result = thing->get_property<bool>(key);
    lua_pushboolean(L, result);

    return 1;
  }

  int thing_get_property_value(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    int result = thing->get_property<int>(key);
    lua_pushinteger(L, result);

    return 1;
  }

  int thing_get_property_string(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    std::string result = thing->get_property<std::string>(key).c_str();
    lua_pushstring(L, result.c_str());

    return 1;
  }

  int thing_queue_action(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if ((num_args < 2))
    {
      CLOG(WARNING, "Lua") << "expected >= 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    std::string action_type = lua_tostring(L, 2);

    if (!Action::exists(action_type))
    {
      MAJOR_ERROR("Lua script requested queue of non-existent Action \"%s\"", action_type.c_str());
      return 0;
    }

    std::unique_ptr<Action> new_action = Action::create(action_type, thing);
    std::vector<ThingRef> objects;

    if (num_args > 2)
    {
      for (int arg_number = 3; arg_number <= num_args; ++arg_number)
      {
        objects.push_back(ThingRef(lua_tointeger(L, arg_number)));
      }
    }

    new_action->set_objects(objects);
    thing->queue_action(std::move(new_action));

    return 1;
  }

  int thing_queue_targeted_action(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if ((num_args < 3))
    {
      CLOG(WARNING, "Lua") << "expected >= 3 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    std::string action_type = lua_tostring(L, 2);
    ThingRef target = ThingRef(lua_tointeger(L, 3));

    if (!Action::exists(action_type))
    {
      MAJOR_ERROR("Lua script requested queue of non-existent Action \"%s\"", action_type.c_str());
      return 0;
    }

    std::unique_ptr<Action> new_action = Action::create(action_type, thing);
    std::vector<ThingRef> objects;

    if (num_args > 3)
    {
      for (int arg_number = 4; arg_number <= num_args; ++arg_number)
      {
        objects.push_back(ThingRef(lua_tointeger(L, arg_number)));
      }
    }

    new_action->set_target(target);
    new_action->set_objects(objects);
    thing->queue_action(std::move(new_action));

    return 1;
  }

  int thing_queue_directional_action(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if ((num_args < 5))
    {
      CLOG(WARNING, "Lua") << "expected >= 5 arguments, got %d" << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    std::string action_type = lua_tostring(L, 2);
    int x = lua_tointeger(L, 3);
    int y = lua_tointeger(L, 4);
    int z = lua_tointeger(L, 5);
    Direction direction{ x, y, z };

    if (!Action::exists(action_type))
    {
      MAJOR_ERROR("Lua script requested queue of non-existent Action \"%s\"", action_type.c_str());
      return 0;
    }

    std::unique_ptr<Action> new_action = Action::create(action_type, thing);
    std::vector<ThingRef> objects;

    if (num_args > 5)
    {
      for (int arg_number = 6; arg_number <= num_args; ++arg_number)
      {
        objects.push_back(ThingRef(lua_tointeger(L, arg_number)));
      }
    }

    new_action->set_target(direction);
    new_action->set_objects(objects);
    thing->queue_action(std::move(new_action));

    return 1;
  }

  int thing_set_property_flag(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 3)
    {
      CLOG(WARNING, "Lua") << "expected 3 arguments, got %d" << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    bool value = (lua_toboolean(L, 3) != 0);
    thing->set_property<bool>(key, value);

    return 0;
  }

  int thing_set_property_value(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 3)
    {
      CLOG(WARNING, "Lua") << "expected 3 arguments, got %d" << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    int value = static_cast<int>(lua_tointeger(L, 3));
    thing->set_property<int>(key, value);

    return 0;
  }

  int thing_set_property_string(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 3)
    {
      CLOG(WARNING, "Lua") << "expected 3 arguments, got %d" << num_args;
      return 0;
    }

    ThingRef thing = ThingRef(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    const char* value = lua_tostring(L, 3);
    std::string svalue = std::string(value);
    thing->set_property<std::string>(key, svalue);

    return 0;
  }

  int thing_move_into(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    ThingRef thing_to_move = ThingRef(lua_tointeger(L, 1));
    ThingRef thing_destination = ThingRef(lua_tointeger(L, 2));

    bool result = thing_to_move->move_into(thing_destination);

    lua_pushboolean(L, static_cast<int>(result));

    return 1;
  }

  void register_functions()
  {
    the_lua_instance.register_function("thing_create", thing_create);
    the_lua_instance.register_function("thing_destroy", thing_destroy);
    the_lua_instance.register_function("thing_get_player", thing_get_player);
    the_lua_instance.register_function("thing_get_coords", thing_get_coords);
    the_lua_instance.register_function("thing_get_type", thing_get_type);
    the_lua_instance.register_function("thing_get_intrinsic_flag", thing_get_intrinsic_flag);
    the_lua_instance.register_function("thing_get_intrinsic_value", thing_get_intrinsic_value);
    the_lua_instance.register_function("thing_get_intrinsic_string", thing_get_intrinsic_string);
    the_lua_instance.register_function("thing_get_property_flag", thing_get_property_flag);
    the_lua_instance.register_function("thing_get_property_value", thing_get_property_value);
    the_lua_instance.register_function("thing_get_property_string", thing_get_property_string);
    the_lua_instance.register_function("thing_queue_action", thing_queue_action);
    the_lua_instance.register_function("thing_queue_targeted_action", thing_queue_targeted_action);
    the_lua_instance.register_function("thing_queue_directional_action", thing_queue_directional_action);
    the_lua_instance.register_function("thing_set_property_flag", thing_set_property_flag);
    the_lua_instance.register_function("thing_set_property_value", thing_set_property_value);
    the_lua_instance.register_function("thing_set_property_string", thing_set_property_string);
    the_lua_instance.register_function("thing_move_into", thing_move_into);
  }
} // end namespace LuaThingFunctions