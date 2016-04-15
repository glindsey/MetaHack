#include "stdafx.h"

#include "GameState.h"

#include "ErrorHandler.h"
#include "LuaObject.h"
#include "Map.h"
#include "MapFactory.h"
#include "New.h"
#include "Thing.h"
#include "ThingManager.h"

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  ASSERT_CONDITION(p_instance == nullptr);

  p_instance = this;

  m_thing_manager.reset(NEW ThingManager());
  m_map_factory.reset(NEW MapFactory());
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

MapFactory& GameState::get_map_factory()
{
  ASSERT_CONDITION(m_map_factory);

  return *(m_map_factory.get());
}

ThingManager& GameState::get_thing_manager()
{
  ASSERT_CONDITION(m_thing_manager);

  return *(m_thing_manager.get());
}

bool GameState::set_player(ThingId ref)
{
  ASSERT_CONDITION(ref != get_thing_manager().get_mu());

  m_player = ref;
  return true;
}

ThingId GameState::get_player() const
{
  return m_player;
}

bool GameState::process_tick()
{
  ThingId player = get_player();

  if (player->action_is_pending() || player->action_is_in_progress())
  {
    // QUESTION: Do we want to update all Things, PERIOD?  In other words, should
    //           other maps keep playing themselves if the player is not on them?
    //           While this would be awesome, I'd imagine the resulting per-turn
    //           lag would quickly grow intolerable.

    // Get the map the player is on.
    MapId current_map_id = player->get_map_id();
    Map& current_map = GAME.get_map_factory().get(current_map_id);

    // Process everything on the map, and increment game clock.
    current_map.process();
    ++m_game_clock;

    // If player can see the map...
    /// @todo IMPLEMENT THIS CHECK
    // (can be done by checking if the location chain for the player is
    //  entirely transparent)
    if (true /* player is directly on a map */)
    {
      // Update map lighting.
      current_map.update_lighting();

      // Update tile vertex array.
      current_map.update_tile_vertices(player);
    }
    return true;
  }
  return false;
}

GameState& GameState::instance()
{
  ASSERT_CONDITION(p_instance);

  return *(p_instance);
}