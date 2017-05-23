#include "stdafx.h"

#include "game/GameState.h"

#include "components/ComponentManager.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "entity/EntityPool.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "Service.h"
#include "services/IGraphicViews.h"
#include "utilities/JSONUtils.h"


// Namespace aliases
namespace fs = boost::filesystem;

GameState* GameState::s_instance = nullptr;

GameState::GameState(json const& j) :
  Object({ GameState::EventClockChanged::id })
{
  initialize(j);
}

GameState::~GameState()
{
  // Destroy the Lua state.
  m_lua.reset();

  s_instance = nullptr;
}

void GameState::initialize(json const& j)
{
  Assert("GameState", s_instance == nullptr, "tried to create more than one GameState instance at a time");
  s_instance = this;

  // Create the Lua state.
  m_lua.reset(NEW Lua());

  // Register Lua functions.
  m_lua->register_function("app_get_frame_counter", App::LUA_get_frame_counter);
  //m_lua->register_function("print", App::LUA_redirect_print);
  m_lua->register_function("messageLog_add", App::LUA_add);
  m_lua->register_function("get_config", App::LUA_get_config);

  if (j.is_object() && j.size() != 0)
  {
    m_components.reset(NEW ComponentManager(*this, j.value("components", json::object())));
    m_entityPool.reset(NEW EntityPool(*this));
    m_mapFactory.reset(NEW MapFactory(*this));
  }
  else
  {
    m_components.reset(NEW ComponentManager(*this, json::object()));
    m_entityPool.reset(NEW EntityPool(*this));
    m_mapFactory.reset(NEW MapFactory(*this));
  }
}

void from_json(json const& j, GameState& obj)
{
  obj.initialize(j);
}

void to_json(json& j, GameState const& obj)
{
  j["components"] = obj.m_components->toJSON();
}

MapFactory& GameState::maps()
{
  return *m_mapFactory;
}

MapFactory const& GameState::maps() const
{
  return *m_mapFactory;
}

EntityPool& GameState::entities()
{
  return *m_entityPool;
}

EntityPool const& GameState::entities() const
{
  return *m_entityPool;
}

ComponentManager & GameState::components()
{
  return *m_components;
}

ComponentManager const& GameState::components() const
{
  return *m_components;
}

bool GameState::processGameClockTick()
{
  EntityId player = components().globals.player();

  if (player->actionIsPending() || player->actionIsInProgress()) ///< @todo check -- second half of this shouldn't be necessary
  {
    // QUESTION: Do we want to update all Entities, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the map the player is on.
    MapID map = COMPONENTS.position[player].map();

    // Process everything on the map.
    MAPS.get(map).processEntities();
    return true;
  }
  return false;
}

Lua& GameState::lua()
{
  return *m_lua;
}

Lua const& GameState::lua() const
{
  return *m_lua;
}


GameState& GameState::instance()
{
  Assert("GameState", s_instance != nullptr, "tried to get non-existent GameState instance");

  return *(s_instance);
}