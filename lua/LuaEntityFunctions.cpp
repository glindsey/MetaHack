#include "stdafx.h"

#include "lua/LuaEntityFunctions.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "Service.h"
#include "services/IGameRules.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"

// === MACROS =================================================================
#define STRINGIFY(x) #x
#define LUA_REGISTER(x) do { GAME.lua().register_function(STRINGIFY(x), x); } while(0)

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
    //json& thing_data = Service<IGameRules>::get().category(new_thing_type);
    bool is_creatable = true; // thing_data.value("creatable", false);

    if (is_creatable)
    {
      new_thing = GAME.entities().create(new_thing_type);
      success = SYSTEMS.spacial().moveEntityInto(new_thing, entity);

      if (success && (num_args > 2))
      {
        /// @todo Possibly disallow adding Quantity component if category 
        ///       doesn't already have it.
        unsigned int quantity = static_cast<unsigned int>(lua_tointeger(L, 3));
        if (quantity > 1)
        {
          COMPONENTS.quantity[new_thing] = quantity;
        }
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

  /// @todo This function and thing_get_location should be merged into one.
  int thing_get_coords(lua_State* L)
  {
    int num_args = lua_gettop(L);

    if (num_args != 1)
    {
      CLOG(WARNING, "Lua") << "expected 1 argument, got " << num_args;
      return 0;
    }

    EntityId entity = EntityId(lua_tointeger(L, 1));

    if (!COMPONENTS.position.existsFor(entity))
    {
      lua_pushnil(L);
      lua_pushnil(L);
    }
    else
    {
      auto& entityPosition = COMPONENTS.position[entity];
      auto coords = entityPosition.coords();
      lua_pushinteger(L, coords.x);
      lua_pushinteger(L, coords.y);
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

    if (!COMPONENTS.position.existsFor(entity))
    {
      lua_pushnil(L);
    }
    else
    {
      auto& entityPosition = COMPONENTS.position[entity];
      auto parent = entityPosition.parent();
      lua_pushinteger(L, static_cast<lua_Integer>(parent));
    }

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
    std::string result = COMPONENTS.category[entity];
    lua_pushstring(L, result.c_str());

    return 1;
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
    auto result = Service<IGameRules>::get().category(COMPONENTS.category[entity]).value(key, json());
    auto slot_count = GAME.lua().push_value(result);

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

    bool result = SYSTEMS.spacial().moveEntityInto(thing_to_move, thing_destination);

    lua_pushboolean(L, static_cast<int>(result));

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
    LUA_REGISTER(thing_get_intrinsic);
    LUA_REGISTER(thing_queue_action);
    LUA_REGISTER(thing_queue_targeted_action);
    LUA_REGISTER(thing_queue_directional_action);
    LUA_REGISTER(thing_move_into);
  }
} // end namespace LuaEntityFunctions