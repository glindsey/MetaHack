#include "stdafx.h"

#include "lua/LuaEntityFunctions.h"

#include "game/GameState.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

// === MACROS =================================================================
#define STRINGIFY(x) #x
#define LUA_REGISTER(x) do { the_lua_instance.register_function(STRINGIFY(x), x); } while(0)

// === FUNCTIONS ==============================================================

namespace LuaEntityFunctions
{
  int thing_create(lua_State* L)
  {
    bool success = false;
    EntityId new_thing;
    int num_args = lua_gettop(L);

    if ((num_args < 2) || (num_args > 3))
    {
      CLOG(WARNING, "Lua") << "expected 2 or 3 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    std::string new_thing_type = lua_tostring(L, 2);

    // Check to make sure the Entity is actually creatable.
    /// @todo Might want the ability to disable this check for debugging purposes?
    Metadata& thing_metadata = GAME.getMetadataCollection("entity").get(new_thing_type);
    bool is_creatable = thing_metadata.getIntrinsic("creatable", Property::from(false)).as<bool>();

    if (is_creatable)
    {
      new_thing = GAME.getEntities().create(new_thing_type);
      success = new_thing->move_into(entity);

      if (success && (num_args > 2))
      {
        unsigned int quantity = static_cast<unsigned int>(lua_tointeger(L, 3));
        new_thing->setQuantity(quantity);
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

    EntityId entity = EntityId(lua_tointeger(L, 1));
    entity->destroy();

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

    EntityId player = GAME.getPlayer();
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

    EntityId entity = EntityId(lua_tointeger(L, 1));

    MapId map_id = GAME.getPlayer()->getMapId();
    auto maptile = entity->getMapTile();

    if (maptile != nullptr)
    {
      auto coords = maptile->getCoords();
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

  int thing_get_location(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    EntityId location = entity->getLocation();
    lua_pushinteger(L, static_cast<lua_Integer>(location));

    return 1;
  }

  int thing_get_type(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    std::string result = entity->getType();
    lua_pushstring(L, result.c_str());

    return 1;
  }

  int thing_get_base_property(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    auto result = entity->getBaseProperty(key);
    auto slot_count = the_lua_instance.push_value(result);

    return slot_count;
  }

  int thing_get_modified_property(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    auto result = entity->getModifiedProperty(key);
    auto slot_count = the_lua_instance.push_value(result);

    return slot_count;
  }

  int thing_get_intrinsic(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 2)
    {
      CLOG(WARNING, "Lua") << "expected 2 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    const char* key = lua_tostring(L, 2);
    auto result = entity->getIntrinsic(key);
    auto slot_count = the_lua_instance.push_value(result);

    return slot_count;
  }

  int thing_queue_action(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if ((num_args < 2))
    {
      CLOG(WARNING, "Lua") << "expected >= 2 arguments, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));
    std::string action_type = lua_tostring(L, 2);

    if (!Actions::Action::exists(action_type))
    {
      CLOG(ERROR, "Lua") << "Lua script requested queue of non-existent Action \"" << action_type << "\"";
      return 0;
    }

    std::unique_ptr<Actions::Action> new_action = Actions::Action::create(action_type, entity);
    std::vector<EntityId> objects;

    if (num_args > 2)
    {
      for (int arg_number = 3; arg_number <= num_args; ++arg_number)
      {
        objects.push_back(EntityId(lua_tointeger(L, arg_number)));
      }
    }

    new_action->setObjects(objects);
    entity->queueAction(std::move(new_action));

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

    EntityId entity = EntityId(lua_tointeger(L, 1));
    std::string action_type = lua_tostring(L, 2);
    EntityId target = EntityId(lua_tointeger(L, 3));

    if (!Actions::Action::exists(action_type))
    {
      CLOG(ERROR, "Lua") << "Lua script requested queue of non-existent Action \"" << action_type << "\"";
      return 0;
    }

    std::unique_ptr<Actions::Action> new_action = Actions::Action::create(action_type, entity);
    std::vector<EntityId> objects;

    if (num_args > 3)
    {
      for (int arg_number = 4; arg_number <= num_args; ++arg_number)
      {
        objects.push_back(EntityId(lua_tointeger(L, arg_number)));
      }
    }

    new_action->setTarget(target);
    new_action->setObjects(objects);
    entity->queueAction(std::move(new_action));

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

    EntityId entity = EntityId(lua_tointeger(L, 1));
    std::string action_type = lua_tostring(L, 2);
    int x = static_cast<int>(lua_tointeger(L, 3));
    int y = static_cast<int>(lua_tointeger(L, 4));
    int z = static_cast<int>(lua_tointeger(L, 5));
    Direction direction{ x, y, z };

    if (!Actions::Action::exists(action_type))
    {
      CLOG(ERROR, "Lua") << "Lua script requested queue of non-existent Action \"" << action_type << "\"";
      return 0;
    }

    std::unique_ptr<Actions::Action> new_action = Actions::Action::create(action_type, entity);
    std::vector<EntityId> objects;

    if (num_args > 5)
    {
      for (int arg_number = 6; arg_number <= num_args; ++arg_number)
      {
        objects.push_back(EntityId(lua_tointeger(L, arg_number)));
      }
    }

    new_action->setTarget(direction);
    new_action->setObjects(objects);
    entity->queueAction(std::move(new_action));

    return 1;
  }

  int thing_set_base_property(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args < 4)
    {
      CLOG(WARNING, "Lua") << "expected >= 4 arguments, got %d" << num_args;
      return 0;
    }

    auto entity = EntityId(lua_tointeger(L, 1));
    auto key = std::string(lua_tostring(L, 2));
    auto type = the_lua_instance.pop_type();
    auto value = the_lua_instance.pop_value(type);

    CLOG(TRACE, "Lua") << "Setting property " << key << 
      " of entity " << entity << " to " << value;
    entity->setBaseProperty(key, value);

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

    EntityId thing_to_move = EntityId(lua_tointeger(L, 1));
    EntityId thing_destination = EntityId(lua_tointeger(L, 2));

    bool result = thing_to_move->move_into(thing_destination);

    lua_pushboolean(L, static_cast<int>(result));

    return 1;
  }

  int thing_add_property_modifier(lua_State* L)
  {
    int num_args = lua_gettop(L);
    bool result = false;

    if ((num_args < 3) || (num_args > 4))
    {
      CLOG(WARNING, "Lua") << "expected 3 or 4 arguments, got " << num_args;
      return 0;
    }

    EntityId thing_being_modified = EntityId(lua_tointeger(L, 1));
    std::string key = lua_tostring(L, 2);
    EntityId thing_doing_the_modifying = EntityId(lua_tointeger(L, 3));
    unsigned int expires_at = (num_args == 4) ? static_cast<unsigned int>(lua_tointeger(L, 4)) : 0;

    /// @todo FIX ME
    //result = thing_being_modified->addModifier(key, thing_doing_the_modifying, ElapsedTime(expires_at));

    lua_pushboolean(L, static_cast<int>(result));

    return 1;
  }

  int thing_remove_property_modifier(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 3)
    {
      CLOG(WARNING, "Lua") << "expected 3 arguments, got " << num_args;
      return 0;
    }

    EntityId thing_being_modified = EntityId(lua_tointeger(L, 1));
    std::string key = lua_tostring(L, 2);
    EntityId thing_doing_the_modifying = EntityId(lua_tointeger(L, 3));

    size_t result = thing_being_modified->removeModifier(key, thing_doing_the_modifying);

    lua_pushinteger(L, static_cast<lua_Integer>(result));

    return 1;
  }

  void registerFunctions()
  {
    LUA_REGISTER(thing_create);
    LUA_REGISTER(thing_destroy);
    LUA_REGISTER(thing_get_player);
    LUA_REGISTER(thing_get_location);
    LUA_REGISTER(thing_get_coords);
    LUA_REGISTER(thing_get_type);
    LUA_REGISTER(thing_get_base_property);
    LUA_REGISTER(thing_get_modified_property);
    LUA_REGISTER(thing_get_intrinsic);
    LUA_REGISTER(thing_queue_action);
    LUA_REGISTER(thing_queue_targeted_action);
    LUA_REGISTER(thing_queue_directional_action);
    LUA_REGISTER(thing_set_base_property);
    LUA_REGISTER(thing_move_into);
    LUA_REGISTER(thing_add_property_modifier);
    LUA_REGISTER(thing_remove_property_modifier);
  }
} // end namespace LuaEntityFunctions