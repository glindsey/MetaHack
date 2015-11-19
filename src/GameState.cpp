#include "GameState.h"

#include <exception>

#include "New.h"
#include "ThingManager.h"

GameState* GameState::p_instance = nullptr;

GameState::GameState()
  :
  m_thing_manager{ NEW ThingManager() }
{
  if (p_instance != nullptr)
  {
    throw std::exception("Two GameState instances are not allowed at once");
  }

  p_instance = this;
}

GameState::~GameState()
{
  p_instance = nullptr;
}

ThingManager& GameState::get_thing_manager()
{
  return *(m_thing_manager.get());
}

GameState& GameState::instance()
{
  if (p_instance == nullptr)
  {
    throw std::exception("No GameState is instantiated");
  }

  return *(p_instance);
}
