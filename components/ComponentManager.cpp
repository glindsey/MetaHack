#include "stdafx.h"

#include "components/ComponentManager.h"

#include "entity/EntityId.h"
#include "game/App.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "lua/LuaTemplates.h"
#include "utilities/JSONUtils.h"

int LUA_get_busy_ticks(lua_State* L)
{
  return LUA_getValue<unsigned int>(L, [&](EntityId entity) -> unsigned int
  {
    return COMPONENTS.activity.existsFor(entity) ? COMPONENTS.activity[entity].busyTicks() : 0;
  });
}


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
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].mass().value() : 0;
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
    return COMPONENTS.quantity.existsFor(entity) ? COMPONENTS.quantity[entity] : 1;
  });
}

int LUA_get_volume(lua_State* L)
{
  return LUA_getValue<int>(L, [&](EntityId entity) -> int
  {
    return COMPONENTS.physical.existsFor(entity) ? COMPONENTS.physical[entity].volume().value() : 0;
  });
}

int LUA_is_lit(lua_State* L)
{
  return LUA_getValue<bool>(L, [&](EntityId entity) -> bool
  {
    return COMPONENTS.lightSource.existsFor(entity) ? COMPONENTS.lightSource[entity].lit() : false;
  });
}

int LUA_set_lit(lua_State* L)
{
  return LUA_setBool(L, [&](EntityId entity, bool value) -> void
  {
    if (COMPONENTS.lightSource.existsFor(entity)) COMPONENTS.lightSource[entity].lit() = value;
  });
}

namespace Components
{

  ComponentManager::ComponentManager(GameState& gameState) :
    m_gameState{ gameState }
  {
    initialize();
  }

  ComponentManager::ComponentManager(GameState& gameState, json const& j) :
    ComponentManager(gameState)
  {
    JSONUtils::doIfPresent(j, "globals", [this](auto const& value) { globals = value; });
    JSONUtils::doIfPresent(j, "activity", [this](auto const& value) { activity = value; });
    JSONUtils::doIfPresent(j, "appearance", [this](auto const& value) { appearance = value; });
    JSONUtils::doIfPresent(j, "bodyparts", [this](auto const& value) { bodyparts = value; });
    JSONUtils::doIfPresent(j, "category", [this](auto const& value) { category = value; });
    JSONUtils::doIfPresent(j, "combustible", [this](auto const& value) { combustible = value; });
    JSONUtils::doIfPresent(j, "corrodible", [this](auto const& value) { corrodible = value; });
    JSONUtils::doIfPresent(j, "digestive-system", [this](auto const& value) { digestiveSystem = value; });
    JSONUtils::doIfPresent(j, "equippable", [this](auto const& value) { equippable = value; });
    JSONUtils::doIfPresent(j, "gender", [this](auto const& value) { gender = value; });
    JSONUtils::doIfPresent(j, "health", [this](auto const& value) { health = value; });
    JSONUtils::doIfPresent(j, "inventory", [this](auto const& value) { inventory = value; });
    JSONUtils::doIfPresent(j, "light-source", [this](auto const& value) { lightSource = value; });
    JSONUtils::doIfPresent(j, "lockable", [this](auto const& value) { lockable = value; });
    JSONUtils::doIfPresent(j, "magical-binding", [this](auto const& value) { magicalBinding = value; });
    JSONUtils::doIfPresent(j, "material-flags", [this](auto const& value) { materialFlags = value; });
    JSONUtils::doIfPresent(j, "matter-state", [this](auto const& value) { matterState = value; });
    JSONUtils::doIfPresent(j, "mobility", [this](auto const& value) { mobility = value; });
    JSONUtils::doIfPresent(j, "openable", [this](auto const& value) { openable = value; });
    JSONUtils::doIfPresent(j, "physical", [this](auto const& value) { physical = value; });
    JSONUtils::doIfPresent(j, "position", [this](auto const& value) { position = value; });
    JSONUtils::doIfPresent(j, "proper-name", [this](auto const& value) { properName = value; });
    JSONUtils::doIfPresent(j, "quantity", [this](auto const& value) { quantity = value; });
    JSONUtils::doIfPresent(j, "sapience", [this](auto const& value) { sapience = value; });
    JSONUtils::doIfPresent(j, "sense-sight", [this](auto const& value) { senseSight = value; });
    JSONUtils::doIfPresent(j, "spacial-memory", [this](auto const& value) { spacialMemory = value; });
  }

  ComponentManager::~ComponentManager()
  {}

  void ComponentManager::initialize()
  {
    m_gameState.lua().register_function("get_busy_ticks", LUA_get_busy_ticks);
    m_gameState.lua().register_function("get_category", LUA_get_category);
    m_gameState.lua().register_function("get_hp", LUA_get_hp);
    m_gameState.lua().register_function("get_mass", LUA_get_mass);
    m_gameState.lua().register_function("get_maxhp", LUA_get_maxhp);
    m_gameState.lua().register_function("get_opacity", LUA_get_opacity);
    m_gameState.lua().register_function("get_proper_name", LUA_get_proper_name);
    m_gameState.lua().register_function("get_quantity", LUA_get_quantity);
    m_gameState.lua().register_function("get_volume", LUA_get_volume);
    m_gameState.lua().register_function("is_lit", LUA_is_lit);
    m_gameState.lua().register_function("set_lit", LUA_set_lit);
  }

  void ComponentManager::clone(EntityId original, EntityId newId)
  {
    activity.cloneIfExists(original, newId);
    appearance.cloneIfExists(original, newId);
    bodyparts.cloneIfExists(original, newId);
    category.cloneIfExists(original, newId);
    combustible.cloneIfExists(original, newId);
    corrodible.cloneIfExists(original, newId);
    digestiveSystem.cloneIfExists(original, newId);
    equippable.cloneIfExists(original, newId);
    gender.cloneIfExists(original, newId);
    health.cloneIfExists(original, newId);
    // Do NOT clone inventory
    lightSource.cloneIfExists(original, newId);
    lockable.cloneIfExists(original, newId);
    magicalBinding.cloneIfExists(original, newId);
    materialFlags.cloneIfExists(original, newId);
    matterState.cloneIfExists(original, newId);
    mobility.cloneIfExists(original, newId);
    openable.cloneIfExists(original, newId);
    physical.cloneIfExists(original, newId);
    position.cloneIfExists(original, newId);
    properName.cloneIfExists(original, newId);
    quantity.cloneIfExists(original, newId);
    sapience.cloneIfExists(original, newId);
    senseSight.cloneIfExists(original, newId);
    spacialMemory.cloneIfExists(original, newId);
  }

  void ComponentManager::erase(EntityId id)
  {
    activity.remove(id);
    appearance.remove(id);
    bodyparts.remove(id);
    category.remove(id);
    combustible.remove(id);
    corrodible.remove(id);
    digestiveSystem.remove(id);
    equippable.remove(id);
    gender.remove(id);
    health.remove(id);
    inventory.remove(id);
    lightSource.remove(id);
    lockable.remove(id);
    magicalBinding.remove(id);
    materialFlags.remove(id);
    matterState.remove(id);
    mobility.remove(id);
    openable.remove(id);
    physical.remove(id);
    position.remove(id);
    properName.remove(id);
    quantity.remove(id);
    sapience.remove(id);
    senseSight.remove(id);
    spacialMemory.remove(id);
  }

  void ComponentManager::populate(EntityId id, json const& j)
  {
    JSONUtils::doIfPresent(j, "activity",         [this, &id](auto const& value) {        activity.update(id, value); });
    JSONUtils::doIfPresent(j, "appearance",       [this, &id](auto const& value) {      appearance.update(id, value); });
    JSONUtils::doIfPresent(j, "bodyparts",        [this, &id](auto const& value) {       bodyparts.update(id, value); });
    JSONUtils::doIfPresent(j, "category",         [this, &id](auto const& value) {        category.update(id, value); });
    JSONUtils::doIfPresent(j, "combustible",      [this, &id](auto const& value) {     combustible.update(id, value); });
    JSONUtils::doIfPresent(j, "corrodible",       [this, &id](auto const& value) {      corrodible.update(id, value); });
    JSONUtils::doIfPresent(j, "digestive-system", [this, &id](auto const& value) { digestiveSystem.update(id, value); });
    JSONUtils::doIfPresent(j, "equippable",       [this, &id](auto const& value) {      equippable.update(id, value); });
    JSONUtils::doIfPresent(j, "gender",           [this, &id](auto const& value) {          gender.update(id, value); });
    JSONUtils::doIfPresent(j, "health",           [this, &id](auto const& value) {          health.update(id, value); });
    JSONUtils::doIfPresent(j, "inventory",        [this, &id](auto const& value) {       inventory.update(id, value); });
    JSONUtils::doIfPresent(j, "light-source",     [this, &id](auto const& value) {     lightSource.update(id, value); });
    JSONUtils::doIfPresent(j, "lockable",         [this, &id](auto const& value) {        lockable.update(id, value); });
    JSONUtils::doIfPresent(j, "magical-binding",  [this, &id](auto const& value) {  magicalBinding.update(id, value); });
    JSONUtils::doIfPresent(j, "material-flags",   [this, &id](auto const& value) {   materialFlags.update(id, value); });
    JSONUtils::doIfPresent(j, "matter-state",     [this, &id](auto const& value) {     matterState.update(id, value); });
    JSONUtils::doIfPresent(j, "mobility",         [this, &id](auto const& value) {        mobility.update(id, value); });
    JSONUtils::doIfPresent(j, "openable",         [this, &id](auto const& value) {        openable.update(id, value); });
    JSONUtils::doIfPresent(j, "physical",         [this, &id](auto const& value) {        physical.update(id, value); });
    JSONUtils::doIfPresent(j, "position",         [this, &id](auto const& value) {        position.update(id, value); });
    JSONUtils::doIfPresent(j, "proper-name",      [this, &id](auto const& value) {      properName.update(id, value); });
    JSONUtils::doIfPresent(j, "quantity",         [this, &id](auto const& value) {        quantity.update(id, value); });
    JSONUtils::doIfPresent(j, "sapience",         [this, &id](auto const& value) {        sapience.update(id, value); });
    JSONUtils::doIfPresent(j, "sense-sight",      [this, &id](auto const& value) {      senseSight.update(id, value); });
    JSONUtils::doIfPresent(j, "spacial-memory",   [this, &id](auto const& value) {   spacialMemory.update(id, value); });
  }

  json ComponentManager::toJSON()
  {
    json j = json::object();

    j["globals"] = globals;
    j["activity"] = activity;
    j["appearance"] = appearance;
    j["bodyparts"] = bodyparts;
    j["category"] = category;
    j["combustible"] = combustible;
    j["equippable"] = equippable;
    j["gender"] = gender;
    j["health"] = health;
    j["inventory"] = inventory;
    j["light-source"] = lightSource;
    j["lockable"] = lockable;
    j["magical-binding"] = magicalBinding;
    j["material-flags"] = materialFlags;
    j["matter-state"] = matterState;
    j["mobility"] = mobility;
    j["openable"] = openable;
    j["physical"] = physical;
    j["position"] = position;
    j["proper-name"] = properName;
    j["sapience"] = sapience;
    j["sense-sight"] = senseSight;
    j["spacial-memory"] = spacialMemory;

    return j;
  }

} // end namespace
