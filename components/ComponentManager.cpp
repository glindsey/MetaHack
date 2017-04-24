#include "stdafx.h"

#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaTemplates.h"
#include "utilities/JSONUtils.h"

int LUA_get_category(lua_State* L)
{
  return LUA_getValue<std::string>(L, [&](EntityId entity) -> std::string
  {
    return COMPONENTS.category.existsFor(entity) ? COMPONENTS.category[entity] : "";
  });
}

int LUA_get_hp(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.health.existsFor(entity) ? COMPONENTS.health[entity].hp() : 0;
  });
}

int LUA_get_mass(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].mass() : 0;
  });
}

int LUA_get_maxhp(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.health.existsFor(entity) ? COMPONENTS.health[entity].maxHp() : 0;
  });
}

int LUA_get_opacity(lua_State* L)
{
  return LUA_getValue<Color>(L, [&](EntityId entity) -> Color
  {
    return COMPONENTS.appearance.existsFor(entity) ? COMPONENTS.appearance[entity].opacity() : Color::White;
  });
}

int LUA_get_quantity(lua_State* L)
{
  return LUA_getValue<unsigned int>(L, [&](EntityId entity) -> unsigned int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].quantity() : 1;
  });
}

int LUA_get_volume(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].volume() : 0;
  });
}

ComponentManager::ComponentManager()
{
  initialize();
}

ComponentManager::ComponentManager(json const& j)
{
  initialize();
  JSONUtils::doIfPresent(j, "appearance", [&](auto const& value) { appearance = value; });
  JSONUtils::doIfPresent(j, "category", [&](auto const& value) { category = value; });
  JSONUtils::doIfPresent(j, "gender", [&](auto const& value) { gender = value; });
  JSONUtils::doIfPresent(j, "health", [&](auto const& value) { health = value; });
  JSONUtils::doIfPresent(j, "inventory", [&](auto& value) { inventory = value; });
  JSONUtils::doIfPresent(j, "physical", [&](auto& value) { physical = value; });
  JSONUtils::doIfPresent(j, "position", [&](auto& value) { position = value; });
  JSONUtils::doIfPresent(j, "spacial-memory", [&](auto& value) { spacialMemory = value; });
}

ComponentManager::~ComponentManager()
{}

void ComponentManager::initialize()
{
  the_lua_instance.register_function("get_category", LUA_get_category);
  the_lua_instance.register_function("get_hp", LUA_get_hp);
  the_lua_instance.register_function("get_mass", LUA_get_mass);
  the_lua_instance.register_function("get_maxhp", LUA_get_maxhp);
  the_lua_instance.register_function("get_opacity", LUA_get_opacity);
  the_lua_instance.register_function("get_quantity", LUA_get_quantity);
  the_lua_instance.register_function("get_volume", LUA_get_volume);
}

void ComponentManager::clone(EntityId original, EntityId newId)
{
  appearance.cloneIfExists(original, newId);
  category.cloneIfExists(original, newId);
  gender.cloneIfExists(original, newId);
  health.cloneIfExists(original, newId);
  // Do NOT clone inventory
  physical.cloneIfExists(original, newId);
  position.cloneIfExists(original, newId);
  spacialMemory.cloneIfExists(original, newId);
}

void ComponentManager::populate(EntityId id, json const& j)
{
  JSONUtils::doIfPresent(j, "appearance", [&](auto& value) { appearance[id] = value; });
  JSONUtils::doIfPresent(j, "category", [&](auto& value) { category[id] = value; });
  JSONUtils::doIfPresent(j, "gender", [&](auto& value) { gender[id] = value; });
  JSONUtils::doIfPresent(j, "health", [&](auto& value) { health[id] = value; });
  JSONUtils::doIfPresent(j, "inventory", [&](auto& value) { inventory[id] = value; });
  JSONUtils::doIfPresent(j, "physical", [&](auto& value) { physical[id] = value; });
  JSONUtils::doIfPresent(j, "position", [&](auto& value) { position[id] = value; });
  JSONUtils::doIfPresent(j, "spacial-memory", [&](auto& value) { spacialMemory[id] = value; });
}

void from_json(json const& j, ComponentManager& obj)
{
  JSONUtils::doIfPresent(j, "appearance", [&](auto& value) { obj.appearance = value; });
  JSONUtils::doIfPresent(j, "category", [&](auto& value) { obj.category = value; });
  JSONUtils::doIfPresent(j, "gender", [&](auto& value) { obj.gender = value; });
  JSONUtils::doIfPresent(j, "health", [&](auto& value) { obj.health = value; });
  JSONUtils::doIfPresent(j, "inventory", [&](auto& value) { obj.inventory = value; });
  JSONUtils::doIfPresent(j, "physical", [&](auto& value) { obj.physical = value; });
  JSONUtils::doIfPresent(j, "position", [&](auto& value) { obj.position = value; });
  JSONUtils::doIfPresent(j, "spacial-memory", [&](auto& value) { obj.spacialMemory = value; });
}

void to_json(json& j, ComponentManager const& obj)
{
  j["appearance"] = obj.appearance;
  j["category"] = obj.category;
  j["gender"] = obj.gender;
  j["health"] = obj.health;
  j["inventory"] = obj.inventory;
  j["physical"] = obj.physical;
  j["position"] = obj.position;
  j["spacial-memory"] = obj.spacialMemory;
}