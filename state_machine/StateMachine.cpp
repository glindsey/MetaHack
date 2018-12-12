#include "stdafx.h"

#include <boost/ptr_container/ptr_map.hpp>

#include "state_machine/StateMachine.h"

#include "state_machine/State.h"

struct StateMachine::Impl
{
  Impl(Object* parent_)
    :
    parent{ parent_ }
  {}

  Object* const parent;
  boost::ptr_map<std::string, State> state_map;
  State* current_state;
  std::string machine_name;
};

StateMachine::StateMachine(std::string const& machine_name,
                           Object* parent)
  :
  RenderableToTexture(),
  Object({}),
  pImpl(NEW Impl(parent))
{
  pImpl->current_state = nullptr;
  pImpl->machine_name = machine_name;
  if (pImpl->parent) subscribeTo(pImpl->parent, EventID::All);
}

StateMachine::~StateMachine()
{
  if (pImpl->parent) unsubscribeFrom(pImpl->parent, EventID::All);
  change_to(nullptr);
}

std::string const& StateMachine::getName()
{
  return pImpl->machine_name;
}

bool StateMachine::add_state(State* state)
{
  Assert("StateMachine", state, "tried to add null state to StateMachine");

  std::string state_name = state->getName();

  if (pImpl->state_map.count(state_name) == 0)
  {
    pImpl->state_map.insert(state_name, state);
    return true;
  }
  else
  {
    CLOG(ERROR, "StateMachine") << "attempted to add state \"" << state_name <<
      "\" to state machine \"" << getName() << "\" more than once";
    return false;
  }
}

bool StateMachine::delete_state(State* state)
{
  Assert("StateMachine", state, "tried to delete null state from StateMachine");

  return delete_state(state->getName());
}

bool StateMachine::delete_state(std::string const& state_name)
{
  if (state_name == pImpl->current_state->getName())
  {
    CLOG(ERROR, "StateMachine") << "attempted to delete state \"" << state_name << "\" while it was the current state";
  }

  if (pImpl->state_map.count(state_name) == 0)
  {
    CLOG(ERROR, "StateMachine") << "attempted to delete state \"" << state_name <<
      "\" to state machine \"" << getName() << "\" when it didn't exist";
    return false;
  }
  else
  {
    pImpl->state_map.erase(state_name);
    return true;
  }
}

void StateMachine::execute()
{
  if (pImpl->current_state != nullptr)
  {
    pImpl->current_state->execute();
  }
}

bool StateMachine::render(sf::RenderTexture& texture, int frame)
{
  if (pImpl->current_state == nullptr)
  {
    return false;
  }
  else
  {
    return pImpl->current_state->render(texture, frame);
  }
}

bool StateMachine::change_to(State* state)
{
  bool terminator_result = true;

  if (pImpl->current_state != nullptr)
  {
    terminator_result = pImpl->current_state->terminate();
    if (terminator_result == false)
    {
      CLOG(WARNING, "StateMachine") << "Terminator for state \"" <<
        pImpl->current_state->getName() << "\" in state machine \"" <<
        this->getName() << "\" returned false";
    }
  }

  if (terminator_result == true)
  {
    pImpl->current_state = state;

    if (state != nullptr)
    {
      bool initializer_result = pImpl->current_state->initialize();
      if (initializer_result == false)
      {
        CLOG(WARNING, "StateMachine") << "Initializer for state \"" <<
          pImpl->current_state->getName() << "\" in state machine \"" <<
          this->getName() << "\" returned false";
      }
    }
  }

  return terminator_result;
}

bool StateMachine::change_to(std::string const& new_state_name)
{
  if (pImpl->state_map.count(new_state_name) == 0)
  {
    CLOG(ERROR, "StateMachine") << "attempted to change to state \"" << new_state_name <<
      "\" in state machine \"" << getName() << "\" when it doesn't exist";
    return false;
  }
  else
  {
    return change_to(&(pImpl->state_map.at(new_state_name)));
  }
}

State* StateMachine::get_current_state()
{
  return pImpl->current_state;
}

std::string const& StateMachine::get_current_state_name()
{
  static std::string const noneDesc = std::string("(none)");

  if (pImpl->current_state == nullptr)
  {
    return noneDesc;
  }
  else
  {
    return pImpl->current_state->getName();
  }
}

bool StateMachine::onEvent(Event const& event)
{
  return false;
}
