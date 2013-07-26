#include "StateMachine.h"

#include <boost/ptr_container/ptr_map.hpp>
#include <string>

#include "ErrorHandler.h"
#include "State.h"

struct StateMachine::Impl
{
  boost::ptr_map<std::string, State> state_map;
  State* current_state;
  std::string machine_name;
};

StateMachine::StateMachine(std::string const& machine_name)
  : impl(new Impl())
{
  impl->current_state = nullptr;
  impl->machine_name = machine_name;

  //ctor
}

StateMachine::~StateMachine()
{
  change_to(nullptr);
  //dtor
}

std::string const& StateMachine::get_name()
{
  return impl->machine_name;
}

bool StateMachine::add_state(State* state)
{
  ASSERT_NOT_NULL(state);

  std::string state_name = state->get_name();

  if (impl->state_map.count(state_name) == 0)
  {
    impl->state_map.insert(state_name, state);
    return true;
  }
  else
  {
    MAJOR_ERROR("Attempted to add state \"%s\" to state machine \"%s\" more than once",
                state_name.c_str(), this->get_name().c_str());
    return false;
  }
}

bool StateMachine::delete_state(State* state)
{
  ASSERT_NOT_NULL(state);

  return delete_state(state->get_name());
}

bool StateMachine::delete_state(std::string const& state_name)
{
  if (state_name == impl->current_state->get_name())
  {
    MAJOR_ERROR("Attempted to delete state \"%s\" while it was the current state",
                state_name.c_str());
  }

  if (impl->state_map.count(state_name) == 0)
  {
    MAJOR_ERROR("Attempted to delete state \"%s\" to state machine \"%s\" when it wasn't in there",
                state_name.c_str(), this->get_name().c_str());
    return false;
  }
  else
  {
    impl->state_map.erase(state_name);
    return true;
  }
}

void StateMachine::execute()
{
  if (impl->current_state != nullptr)
  {
    impl->current_state->execute();
  }
}

bool StateMachine::render(sf::RenderTarget& target, int frame)
{
  if (impl->current_state == nullptr)
  {
    return false;
  }
  else
  {
    return impl->current_state->render(target, frame);
  }
}

EventResult StateMachine::handle_event(sf::Event& event)
{
  if (impl->current_state == nullptr)
  {
    return EventResult::Ignored;
  }
  else
  {
    return impl->current_state->handle_event(event);
  }
}

bool StateMachine::change_to(State* state)
{
  bool terminator_result = true;

  if (impl->current_state != nullptr)
  {
    terminator_result = impl->current_state->terminate();
    if (terminator_result == false)
    {
      MINOR_ERROR("Terminator for state \"%s\" in state machine \"%s\" returned false",
                  impl->current_state->get_name().c_str(),
                  this->get_name().c_str());
    }
  }

  if (terminator_result == true)
  {
    impl->current_state = state;

    if (state != nullptr)
    {
      bool initializer_result = impl->current_state->initialize();
      if (initializer_result == false)
      {
        MINOR_ERROR("Initializer for state \"%s\" in state machine \"%s\" returned false",
                    impl->current_state->get_name().c_str(),
                    this->get_name().c_str());
      }
    }
  }

  return terminator_result;
}

bool StateMachine::change_to(std::string const& new_state_name)
{
  if (impl->state_map.count(new_state_name) == 0)
  {
    MAJOR_ERROR("Attempted to change to state \"%s\" to state machine \"%s\" when it wasn't in there",
                new_state_name.c_str(), this->get_name().c_str());
    return false;
  }
  else
  {
    return change_to(&(impl->state_map.at(new_state_name)));
  }
}

State* StateMachine::get_current_state()
{
  return impl->current_state;
}

std::string const& StateMachine::get_current_state_name()
{
  static std::string const noneDesc = std::string("(none)");

  if (impl->current_state == nullptr)
  {
    return noneDesc;
  }
  else
  {
    return impl->current_state->get_name();
  }
}
