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

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  initialize({});
}

GameState::GameState(json const& j)
{
  initialize(j);
}

GameState::~GameState()
{
  p_instance = nullptr;
}

void GameState::initialize(json const& j)
{
  Assert("GameState", p_instance == nullptr, "tried to create more than one GameState instance at a time");

  p_instance = this;

  if (j.is_object() && j.size() != 0)
  {
    m_global = j.value("global", json::object());
    m_componentsManager.reset(NEW ComponentManager(j.value("components", json::object())));
    m_entityPool.reset(NEW EntityPool(*this));
    m_mapFactory.reset(NEW MapFactory(*this));
  }
  else
  {
    m_global = json::object();
    m_componentsManager.reset(NEW ComponentManager(json::object()));
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
  j["global"] = obj.m_global;
  j["components"] = *(obj.m_componentsManager);
}

MapFactory& GameState::maps()
{
  return *m_mapFactory;
}

EntityPool& GameState::entities()
{
  return *m_entityPool;
}

ComponentManager & GameState::components()
{
  return *m_componentsManager;
}

ElapsedTime GameState::getGameClock() const
{
  return m_global.value("clock", 0ULL);
}

void GameState::setGameClock(ElapsedTime game_clock)
{
  m_global["clock"] = game_clock;
}

void GameState::incrementGameClock(ElapsedTime added_time)
{
  /// @todo Check for the unlikely, but not impossible, chance of rollover.
  setGameClock(getGameClock() + added_time);
}

bool GameState::setPlayer(EntityId ref)
{
  Assert("GameState", ref != EntityId::Mu(), "tried to make nothingness the player");

  m_global["player"] = ref;
  return true;
}

EntityId GameState::getPlayer() const
{
  return m_global.value("player", EntityId());
}

bool GameState::processGameClockTick()
{
  EntityId player = getPlayer();

  auto player_action_pending = player->anyActionIsPending();
  auto player_action_in_progress = player->actionIsInProgress();
  if (player_action_pending || player_action_in_progress)
  {
    // QUESTION: Do we want to update all Entities, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the map the player is on.
    MapId map = COMPONENTS.position[player].map();

    // Process everything on the map, and increment game clock.
    map->processEntities();
    incrementGameClock(ElapsedTime(1));

    // If player can see the map...
    /// @todo IMPLEMENT THIS CHECK
    // (can be done by checking if the location chain for the player is
    //  entirely transparent)
    if (true /* player is directly on a map */)
    {
      // Update map lighting.
      map->updateLighting();
    }
    return true;
  }
  return false;
}

GameState& GameState::instance()
{
  Assert("GameState", p_instance != nullptr, "tried to get non-existent GameState instance");

  return *(p_instance);
}