#include "stdafx.h"

#include "game/GameState.h"

#include "entity/Entity.h"
#include "entity/EntityPool.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "map/MapFactory.h"

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  Assert("GameState", p_instance == nullptr, "tried to create more than one GameState instance at a time");

  p_instance = this;

  m_entity_pool.reset(NEW EntityPool(*this));
  m_map_factory.reset(NEW MapFactory(*this));

  m_data["types"] = json::object({});
  m_data["entities"] = json::object({});
  m_data["global"] = json::object({});
}

GameState::GameState(FileName filename)
{
#if 0
  Assert("GameState", p_instance == nullptr, "tried to create more than one GameState instance at a time");

  p_instance = this;

  std::ifstream fs{ filename.c_str() };
  cereal::XMLInputArchive iarchive{ fs };

  iarchive(m_map_factory, m_entity_pool, m_player);
#endif
}

GameState::~GameState()
{
  p_instance = nullptr;
}

void GameState::saveState(FileName filename)
{
#if 0
  std::ofstream fs{ filename.c_str() };
  cereal::XMLOutputArchive oarchive{ fs };

  oarchive(m_map_factory, m_entity_pool, m_player);
#endif
}

MapFactory& GameState::getMaps()
{
  Assert("GameState", m_map_factory, "MapFactory does not exist");

  return *m_map_factory;
}

EntityPool& GameState::getEntities()
{
  Assert("GameState", m_entity_pool, "EntityPool does not exist");

  return *m_entity_pool;
}

MetadataCollection & GameState::getMetadataCollection(std::string category)
{
  if (m_metacollection.count(category) == 0)
  {
    m_metacollection.insert(category, NEW MetadataCollection(category));
  }

  return m_metacollection.at(category);
}

ElapsedTime GameState::getGameClock() const
{
  return m_data["global"].value("clock", 0ULL);
}

void GameState::setGameClock(ElapsedTime game_clock)
{
  m_data["global"]["clock"] = game_clock;
}

void GameState::incrementGameClock(ElapsedTime added_time)
{
  /// @todo Check for the unlikely, but not impossible, chance of rollover.
  setGameClock(getGameClock() + added_time);
}

bool GameState::setPlayer(EntityId ref)
{
  Assert("GameState", ref != getEntities().get_mu(), "tried to make nothingness the player");

  m_data["global"]["player"] = ref;
  return true;
}

EntityId GameState::getPlayer() const
{
  return m_data["global"].value("player", EntityId());
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
    MapId current_map_id = player->getMapId();
    Map& current_map = GAME.getMaps().get(current_map_id);

    // Process everything on the map, and increment game clock.
    current_map.processEntities();
    incrementGameClock(ElapsedTime(1));

    // If player can see the map...
    /// @todo IMPLEMENT THIS CHECK
    // (can be done by checking if the location chain for the player is
    //  entirely transparent)
    if (true /* player is directly on a map */)
    {
      // Update map lighting.
      current_map.updateLighting();
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