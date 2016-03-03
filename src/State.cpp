#include "stdafx.h"

#include "State.h"

#include "App.h"
#include "ErrorHandler.h"
#include "StateMachine.h"

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
                m_state_machine.get_name().c_str(),
                this->get_name().c_str());
    return false;
  }
}