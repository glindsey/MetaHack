#include "stdafx.h"

#include "game/GameState.h"

#include "ErrorHandler.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "map/MapFactory.h"
#include "entity/Entity.h"
#include "entity/EntityPool.h"

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  ASSERT_CONDITION(p_instance == nullptr);

  p_instance = this;

  m_thing_manager.reset(NEW EntityPool(*this));
  m_map_factory.reset(NEW MapFactory(*this));
}

GameState::GameState(FileName filename)
{
#if 0
  ASSERT_CONDITION(p_instance == nullptr);

  p_instance = this;

  std::ifstream fs{ filename.c_str() };
  cereal::XMLInputArchive iarchive{ fs };

  iarchive(m_map_factory, m_thing_manager, m_player);
#endif
}

GameState::~GameState()
{
  p_instance = nullptr;
}

void GameState::save_state(FileName filename)
{
#if 0
  std::ofstream fs{ filename.c_str() };
  cereal::XMLOutputArchive oarchive{ fs };

  oarchive(m_map_factory, m_thing_manager, m_player);
#endif
}

MapFactory& GameState::get_maps()
{
  ASSERT_CONDITION(m_map_factory);

  return *m_map_factory;
}

EntityPool& GameState::get_entities()
{
  ASSERT_CONDITION(m_thing_manager);

  return *m_thing_manager;
}

MetadataCollection & GameState::get_metadata_collection(std::string category)
{
  if (m_metacollection.count(category) == 0)
  {
    m_metacollection.insert(category, NEW MetadataCollection(category));
  }

  return m_metacollection.at(category);
}

ElapsedTime const & GameState::get_game_clock() const
{
  return m_game_clock;
}

void GameState::set_game_clock(ElapsedTime game_clock)
{
  m_game_clock = game_clock;
}

void GameState::increment_game_clock(ElapsedTime added_time)
{
  /// @todo Check for the unlikely, but not impossible, chance of rollover.
  m_game_clock += added_time;
}

bool GameState::set_player(EntityId ref)
{
  ASSERT_CONDITION(ref != get_entities().get_mu());

  m_player = ref;
  return true;
}

EntityId GameState::get_player() const
{
  return m_player;
}

bool GameState::process_tick()
{
  EntityId player = get_player();

  if (player->action_is_pending() || player->action_is_in_progress())
  {
    // QUESTION: Do we want to update all Entities, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the map the player is on.
    MapId current_map_id = player->get_map_id();
    Map& current_map = GAME.get_maps().get(current_map_id);

    // Process everything on the map, and increment game clock.
    current_map.process();
    increment_game_clock(ElapsedTime(1));

    // If player can see the map...
    /// @todo IMPLEMENT THIS CHECK
    // (can be done by checking if the location chain for the player is
    //  entirely transparent)
    if (true /* player is directly on a map */)
    {
      // Update map lighting.
      current_map.update_lighting();
    }
    return true;
  }
  return false;
}

GameState& GameState::instance()
{
  ASSERT_CONDITION(p_instance != nullptr);

  return *(p_instance);
}