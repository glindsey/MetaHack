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

int LUA_get_proper_name(lua_State* L)
{
  return LUA_getValue<std::string>(L, [&](EntityId entity) -> std::string
  {
    return COMPONENTS.properName.existsFor(entity) ? COMPONENTS.properName[entity] : "";
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

int LUA_is_lit(lua_State* L)
{
  return LUA_getValue<bool>(L, [&](EntityId entity) -> bool
  {
    return COMPONENTS.lightSource.existsFor(entity) ? COMPONENTS.lightSource[entity].lit() : false;
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
  JSONUtils::doIfPresent(j, "bodyparts", [&](auto const& value) { bodyparts = value; });
  JSONUtils::doIfPresent(j, "category", [&](auto const& value) { category = value; });
  JSONUtils::doIfPresent(j, "gender", [&](auto const& value) { gender = value; });
  JSONUtils::doIfPresent(j, "health", [&](auto const& value) { health = value; });
  JSONUtils::doIfPresent(j, "inventory", [&](auto& value) { inventory = value; });
  JSONUtils::doIfPresent(j, "light-source", [&](auto& value) { lightSource = value; });
  JSONUtils::doIfPresent(j, "lockable", [&](auto& value) { lockable = value; });
  JSONUtils::doIfPresent(j, "magical-binding", [&](auto& value) { magicalBinding = value; });
  JSONUtils::doIfPresent(j, "mobility", [&](auto& value) { mobility = value; });
  JSONUtils::doIfPresent(j, "openable", [&](auto& value) { openable = value; });
  JSONUtils::doIfPresent(j, "physical", [&](auto& value) { physical = value; });
  JSONUtils::doIfPresent(j, "position", [&](auto& value) { position = value; });
  JSONUtils::doIfPresent(j, "proper-name", [&](auto& value) { properName = value; });
  JSONUtils::doIfPresent(j, "sense-sight", [&](auto& value) { senseSight = value; });
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
  the_lua_instance.register_function("get_proper_name", LUA_get_proper_name);
  the_lua_instance.register_function("get_quantity", LUA_get_quantity);
  the_lua_instance.register_function("get_volume", LUA_get_volume);
  the_lua_instance.register_function("is_lit", LUA_is_lit);
}

void ComponentManager::clone(EntityId original, EntityId newId)
{
  appearance.cloneIfExists(original, newId);
  bodyparts.cloneIfExists(original, newId);
  category.cloneIfExists(original, newId);
  gender.cloneIfExists(original, newId);
  health.cloneIfExists(original, newId);
  // Do NOT clone inventory
  lightSource.cloneIfExists(original, newId);
  lockable.cloneIfExists(original, newId);
  magicalBinding.cloneIfExists(original, newId);
  mobility.cloneIfExists(original, newId);
  openable.cloneIfExists(original, newId);
  physical.cloneIfExists(original, newId);
  position.cloneIfExists(original, newId);
  properName.cloneIfExists(original, newId);
  senseSight.cloneIfExists(original, newId);
  spacialMemory.cloneIfExists(original, newId);
}

void ComponentManager::populate(EntityId id, json const& j)
{
  JSONUtils::doIfPresent(j, "appearance", [&](auto& value) { appearance[id] = value; });
  JSONUtils::doIfPresent(j, "bodyparts", [&](auto& value) { bodyparts[id] = value; });
  JSONUtils::doIfPresent(j, "category", [&](auto& value) { category[id] = value; });
  JSONUtils::doIfPresent(j, "gender", [&](auto& value) { gender[id] = value; });
  JSONUtils::doIfPresent(j, "health", [&](auto& value) { health[id] = value; });
  JSONUtils::doIfPresent(j, "inventory", [&](auto& value) { inventory[id] = value; });
  JSONUtils::doIfPresent(j, "light-source", [&](auto& value) { lightSource[id] = value; });
  JSONUtils::doIfPresent(j, "lockable", [&](auto& value) { lockable[id] = value; });
  JSONUtils::doIfPresent(j, "magical-binding", [&](auto& value) { magicalBinding[id] = value; });
  JSONUtils::doIfPresent(j, "mobility", [&](auto& value) { mobility[id] = value; });
  JSONUtils::doIfPresent(j, "openable", [&](auto& value) { openable[id] = value; });
  JSONUtils::doIfPresent(j, "physical", [&](auto& value) { physical[id] = value; });
  JSONUtils::doIfPresent(j, "position", [&](auto& value) { position[id] = value; });
  JSONUtils::doIfPresent(j, "proper-name", [&](auto& value) { properName[id] = value.get<std::string>(); });
  JSONUtils::doIfPresent(j, "sense-sight", [&](auto& value) { senseSight[id] = value; });
  JSONUtils::doIfPresent(j, "spacial-memory", [&](auto& value) { spacialMemory[id] = value; });
}

void from_json(json const& j, ComponentManager& obj)
{
  JSONUtils::doIfPresent(j, "appearance", [&](auto& value) { obj.appearance = value; });
  JSONUtils::doIfPresent(j, "bodyparts", [&](auto& value) { obj.bodyparts = value; });
  JSONUtils::doIfPresent(j, "category", [&](auto& value) { obj.category = value; });
  JSONUtils::doIfPresent(j, "gender", [&](auto& value) { obj.gender = value; });
  JSONUtils::doIfPresent(j, "health", [&](auto& value) { obj.health = value; });
  JSONUtils::doIfPresent(j, "inventory", [&](auto& value) { obj.inventory = value; });
  JSONUtils::doIfPresent(j, "light-source", [&](auto& value) { obj.lightSource = value; });
  JSONUtils::doIfPresent(j, "lockable", [&](auto& value) { obj.lockable = value; });
  JSONUtils::doIfPresent(j, "magical-binding", [&](auto& value) { obj.magicalBinding = value; });
  JSONUtils::doIfPresent(j, "mobility", [&](auto& value) { obj.mobility = value; });
  JSONUtils::doIfPresent(j, "openable", [&](auto& value) { obj.openable = value; });
  JSONUtils::doIfPresent(j, "physical", [&](auto& value) { obj.physical = value; });
  JSONUtils::doIfPresent(j, "position", [&](auto& value) { obj.position = value; });
  JSONUtils::doIfPresent(j, "proper-name", [&](auto& value) { obj.properName = value; });
  JSONUtils::doIfPresent(j, "sense-sight", [&](auto& value) { obj.senseSight = value; });
  JSONUtils::doIfPresent(j, "spacial-memory", [&](auto& value) { obj.spacialMemory = value; });
}

void to_json(json& j, ComponentManager const& obj)
{
  j["appearance"] = obj.appearance;
  j["bodyparts"] = obj.bodyparts;
  j["category"] = obj.category;
  j["gender"] = obj.gender;
  j["health"] = obj.health;
  j["inventory"] = obj.inventory;
  j["light-source"] = obj.lightSource;
  j["lockable"] = obj.lockable;
  j["magical-binding"] = obj.magicalBinding;
  j["mobility"] = obj.mobility;
  j["openable"] = obj.openable;
  j["physical"] = obj.physical;
  j["position"] = obj.position;
  j["proper-name"] = obj.properName;
  j["sense-sight"] = obj.senseSight;
  j["spacial-memory"] = obj.spacialMemory;
}