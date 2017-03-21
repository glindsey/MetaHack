#include "stdafx.h"

#include "state_machine/State.h"

#include "game/App.h"
#include "state_machine/StateMachine.h"

State::State(StateMachine& state_machine)
  :
  m_state_machine{ state_machine }
{
  m_state_machine.addObserver(*this, EventID::All);
}

State::~State()
{
  m_state_machine.removeObserver(*this, EventID::All);
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

std::unordered_set<EventID> State::registeredEvents() const
{
  /// @note May want to do this in the subclasses instead.
  ///       Not every state will necessarily forward every event type.
  auto events = Subject::registeredEvents();
  auto& forwarder_events = m_state_machine.registeredEvents();
  events.insert(forwarder_events.begin(), forwarder_events.end());

  return events;
}
