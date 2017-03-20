#include "stdafx.h"

#include "state_machine/State.h"

#include "game/App.h"
#include "ErrorHandler.h"
#include "state_machine/StateMachine.h"

State::State(StateMachine& state_machine)
  :
  m_state_machine{ state_machine }
{}

State::~State()
{}

bool State::change_to(std::string const& new_state)
{
  // Ask the state machine to change to the requested state.
  if (m_state_machine.change_to(new_state))
  {
    return true;
  }
  else
  {
    MAJOR_ERROR("State manager \"%s\" could not change to new state \"%s\"",
                m_state_machine.getName().c_str(),
                this->getName().c_str());
    return false;
  }
}