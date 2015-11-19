#include "GameState.h"

#include <exception>

#include "ErrorHandler.h"
#include "MapFactory.h"
#include "New.h"
#include "ThingManager.h"

GameState* GameState::p_instance = nullptr;

GameState::GameState()
{
  ASSERT_CONDITION(p_instance == nullptr);

  p_instance = this;

  m_thing_manager.reset(NEW ThingManager(*this));
  m_map_factory.reset(NEW MapFactory(*this));
}

GameState::~GameState()
{
  p_instance = nullptr;
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

GameState& GameState::instance()
{
  ASSERT_CONDITION(p_instance);

  return *(p_instance);
}
