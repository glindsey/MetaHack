#include "stdafx.h"

#include "game/AppState.h"

#include "game/App.h"
#include "state_machine/StateMachine.h"

AppState::AppState(StateMachine& state_machine,
                   std::unordered_set<EventID> const events,
                   std::string name)
  :
  State{ state_machine, events, name }
{
}

AppState::~AppState()
{
}

bool AppState::render(sf::RenderTexture& texture, int frame)
{
  texture.clear();

  texture.display();
  return true;
}
