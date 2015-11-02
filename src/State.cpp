#include "State.h"

#include "App.h"
#include "ErrorHandler.h"
#include "StateMachine.h"

#include <boost/log/trivial.hpp>

struct State::Impl
{
  Impl(StateMachine& state_machine_)
    : state_machine(state_machine_) {}

  // State machine that this state belongs to.
  StateMachine& state_machine;
};

State::State(StateMachine& state_machine)
  : pImpl(NEW Impl(state_machine))
{}

State::~State()
{
  //dtor
}

bool State::change_to(std::string const& new_state)
{
  // Ask the state machine to change to the requested state.
  if (pImpl->state_machine.change_to(new_state))
  {
    return true;
  }
  else
  {
    MAJOR_ERROR("State manager \"%s\" could not change to new state \"%s\"",
                pImpl->state_machine.get_name().c_str(),
                this->get_name().c_str());
    return false;
  }
}
