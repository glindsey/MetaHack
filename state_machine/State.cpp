#include "stdafx.h"

#include "state_machine/State.h"

#include "game/App.h"
#include "state_machine/StateMachine.h"

State::State(StateMachine& state_machine,
             std::unordered_set<EventID> const events)
  :
  RenderableToTexture(),
  Observer(),
  Subject(events),
  m_state_machine{ state_machine }
{
}

State::~State()
{
}

bool State::change_to(std::string const& new_state)
{
  // Ask the state machine to change to the requested state.
  if (m_state_machine.change_to(new_state))
  {
    return true;
  }
  else
  {
    CLOG(ERROR, "StateMachine") << "State machine \"" << m_state_machine.getName() <<
      "\" could not change to new state \"" << getName() << "\"";
    return false;
  }
}

StateMachine & State::getStateMachine()
{
  return m_state_machine;
}
