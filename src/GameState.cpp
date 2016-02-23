#include "GameState.h"

#include <exception>
#include <fstream>

#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>

#include "ErrorHandler.h"
#include "MapFactory.h"
#include "New.h"
#include "ThingManager.h"

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  ASSERT_CONDITION(p_instance == nullptr);

  p_instance = this;

  m_thing_manager.reset(NEW ThingManager());
  m_map_factory.reset(NEW MapFactory());
}

GameState::GameState(std::string filename)
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

void GameState::save_state(std::string filename)
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

bool GameState::set_player(ThingRef ref)
{
  ASSERT_CONDITION(ref != get_thing_manager().get_mu());

  m_player = ref;
  return true;
}

ThingRef GameState::get_player() const
{
  return m_player;
}

GameState& GameState::instance()
{
  ASSERT_CONDITION(p_instance);

  return *(p_instance);
}