#include "stdafx.h"

#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"

int LUA_get_quantity(lua_State* L)
{
  int num_args = lua_gettop(L);

  if (num_args != 1)
  {
    CLOG(WARNING, "Lua") << "expected 1 arguments, got " << num_args;
    return 0;
  }

  EntityId entity = EntityId(lua_tointeger(L, 1));

  auto result = COMPONENTS.quantity.value(entity, 1);
  auto slot_count = the_lua_instance.push_value(result);

  return slot_count;
}

ComponentManager::ComponentManager()
{
  the_lua_instance.register_function("get_quantity", LUA_get_quantity);
}

ComponentManager::~ComponentManager()
{}

void ComponentManager::populate(EntityId id, json const& j)
{
  if (j.count("position") != 0) position[id] = j["position"];
  if (j.count("quantity") != 0) quantity[id] = j["quantity"];
}

void from_json(json const& j, ComponentManager& obj)
{
  obj.position = j.value("position", json::object());
  obj.quantity = j.value("quantity", json::object());
}

void to_json(json& j, ComponentManager const& obj)
{
  j["position"] = obj.position;
  j["quantity"] = obj.quantity;
}

