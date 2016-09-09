#include "stdafx.h"

#include "StateMachine.h"

#include "ErrorHandler.h"
#include "State.h"

struct StateMachine::Impl
{
  boost::ptr_map<std::string, State> state_map;
  State* current_state;
  std::string machine_name;
};

StateMachine::StateMachine(std::string const& machine_name)
  : pImpl(NEW Impl())
{
  SET_UP_LOGGER("StateMachine", true);

  pImpl->current_state = nullptr;
  pImpl->machine_name = machine_name;

  //ctor
}

StateMachine::~StateMachine()
{
  change_to(nullptr);
  //dtor
}

std::string const& StateMachine::get_name()
{
  return pImpl->machine_name;
}

bool StateMachine::add_state(State* state)
{
  ASSERT_NOT_NULL(state);

  std::string state_name = state->get_name();

  if (pImpl->state_map.count(state_name) == 0)
  {
    pImpl->state_map.insert(state_name, state);
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
  if (state_name == pImpl->current_state->get_name())
  {
    MAJOR_ERROR("Attempted to delete state \"%s\" while it was the current state",
                state_name.c_str());
  }

  if (pImpl->state_map.count(state_name) == 0)
  {
    MAJOR_ERROR("Attempted to delete state \"%s\" to state machine \"%s\" when it wasn't in there",
                state_name.c_str(), this->get_name().c_str());
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

SFMLEventResult StateMachine::handle_sfml_event(sf::Event& event)
{
  if (pImpl->current_state == nullptr)
  {
    return SFMLEventResult::Ignored;
  }
  else
  {
    return pImpl->current_state->handle_sfml_event(event);
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
        pImpl->current_state->get_name() << "\" in state machine \"" <<
        this->get_name() << "\" returned false";
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
          pImpl->current_state->get_name() << "\" in state machine \"" <<
          this->get_name() << "\" returned false";
      }
    }
  }

  return terminator_result;
}

bool StateMachine::change_to(std::string const& new_state_name)
{
  if (pImpl->state_map.count(new_state_name) == 0)
  {
    MAJOR_ERROR("Attempted to change to state \"%s\" to state machine \"%s\" when it wasn't in there",
                new_state_name.c_str(), this->get_name().c_str());
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
    return pImpl->current_state->get_name();
  }
}