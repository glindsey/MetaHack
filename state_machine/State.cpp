#include "stdafx.h"

#include "state_machine/State.h"

#include "game/App.h"
#include "state_machine/StateMachine.h"

State::State(StateMachine& state_machine,
             std::unordered_set<EventID> const events,
             std::string name)
  :
  RenderableToTexture(),
  Object(events, name),
  m_stateMachine{ state_machine }
{}

State::~State()
{}

bool State::change_to(std::string const& new_state)
{
  // Ask the state machine to change to the requested state.
  if (m_stateMachine.change_to(new_state))
  {
    return true;
  }
  else
  {
    CLOG(ERROR, "StateMachine") << "State machine \"" << m_stateMachine.getName() <<
      "\" could not change to new state \"" << getName() << "\"";
    return false;
  }
}

StateMachine & State::getStateMachine()
{
  return m_stateMachine;
}
