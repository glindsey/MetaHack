#include "Action.h"

#include <algorithm>

#include "MessageLog.h"
#include "Thing.h"
#include "ThingManager.h"
#include "ThingRef.h"

Action::Action()
  :
  m_type{ Action::Type::None },
  m_state{ Action::State::Pending },
  m_things{ std::vector<ThingRef>{ } },
  m_target_thing{ ThingManager::get_mu() },
  m_target_direction{ Direction::None },
  m_quantity{ 0 }
{}

Action::Action(Action::Type type)
  :
  m_type{ type },
  m_state{ Action::State::Pending },
  m_things{ std::vector<ThingRef>{ } },
  m_target_thing{ ThingManager::get_mu() },
  m_target_direction{ Direction::None },
  m_quantity{ 0 }
{}

void Action::set_type(Action::Type type)
{
  m_type = type;
}

Action::Type Action::get_type() const
{
  return m_type;
}

void Action::set_things(std::vector<ThingRef> things)
{
  m_things = things;
}

std::vector<ThingRef> const& Action::get_things() const
{
  return m_things;
}

void Action::add_thing(ThingRef thing)
{
  m_things.push_back(thing);
}

bool Action::remove_thing(ThingRef thing)
{
  auto& iter = std::find(m_things.begin(), m_things.end(), thing);
  if (iter != m_things.end())
  {
    m_things.erase(iter);
    return true;
  }
  return false;
}

void Action::clear_things()
{
  m_things.clear();
}

bool Action::process(ThingRef actor, AnyMap params)
{
  // If entity is currently busy, decrement by one and return.
  int counter_busy = actor->get_property<int>("counter_busy");
  if (counter_busy > 0)
  {
    --counter_busy;
    actor->set_property<int>("counter_busy", counter_busy);
    return true;
  }
  
  // Perform any type-specific processing.
  // Useful if, for example, your Entity can rise from the dead.
  /// @todo Figure out how to implement this safely.
  //actor->call_lua_function("process", {});

  // This uses subsequent if statements rather than a switch,
  // because an action could conceivably run through several (or all)
  // of these states in a single call to process().

  if (m_state == Action::State::Pending)
  {
    bool success = prebegin_(actor, params);
    if (success)
    {
      set_state(Action::State::PreBegin);
    }
    else
    {
      set_state(Action::State::Processed);
    }
  }

  if (m_state == Action::State::PreBegin)
  {
    bool success = begin_(actor, params);
    if (success)
    {
      set_state(Action::State::InProgress);
    }
    else
    {
      set_state(Action::State::Processed);
    }
  }

  if (m_state == Action::State::InProgress)
  {
    finish_(actor, params);
    set_state(Action::State::PostFinish);
  }

  if (m_state == Action::State::Interrupted)
  {
    abort_(actor, params);
    set_state(Action::State::PostFinish);
  }

  if (m_state == Action::State::PostFinish)
  {
    /// Pop the action off of the queue; it's done.
    set_state(Action::State::Processed);
  }

  if (m_state == Action::State::Processed)
  {
    return true;
  }

  return false;
}

bool Action::prebegin_(ThingRef actor, AnyMap params)
{
  /// @todo Set counter_busy based on the action being taken and
  ///       the entity's reflexes.
  return true;
}

bool Action::begin_(ThingRef actor, AnyMap params)
{
  bool success = false;
  unsigned int number_of_things = get_things().size();
  unsigned int action_time = 0;

  switch (get_type())
  {
  case Action::Type::Wait:
    success = actor->do_move(Direction::Self, action_time);
    if (success)
    {
      actor->add_to_property<int>("counter_busy", action_time);
    }
    break;

  case Action::Type::Move:
    success = actor->do_move(get_target_direction(), action_time);
    if (success)
    {
      actor->add_to_property<int>("counter_busy", action_time);
    }
    break;

  case Action::Type::Attack:
    success = actor->do_attack(get_target_direction(), action_time);
    if (success)
    {
      actor->add_to_property<int>("counter_busy", action_time);
    }
    break;

  case Action::Type::Drop:
    for (ThingRef thing : get_things())
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_drop(thing, action_time);
        if (success)
        {
          actor->add_to_property<int>("counter_busy", action_time);
        }
      }
    }
    break;

  case Action::Type::Eat:
    for (ThingRef thing : get_things())
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_eat(thing, action_time);
        if (success)
        {
          actor->add_to_property<int>("counter_busy", action_time);
        }
      }
    }
    break;

  case Action::Type::Get:
    for (ThingRef thing : get_things())
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_pick_up(thing, action_time);
        if (success)
        {
          actor->add_to_property<int>("counter_busy", action_time);
        }
      }
    }
    break;

  case Action::Type::Quaff:
    for (ThingRef thing : get_things())
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_drink(thing, action_time);
        if (success)
        {
          actor->add_to_property<int>("counter_busy", action_time);
        }
      }
    }
    break;

  case Action::Type::PutInto:
  {
    ThingRef container = get_target_thing();
    if (container != ThingManager::get_mu())
    {
      if (container->get_intrinsic<int>("inventory_size") != 0)
      {
        for (ThingRef thing : get_things())
        {
          if (thing != ThingManager::get_mu())
          {
            success = actor->do_put_into(thing, container, action_time);
            if (success)
            {
              actor->add_to_property<int>("counter_busy", action_time);
            }
          }
        }
      }
    }
    else
    {
      the_message_log.add("That target is not a container.");
    }
    break;
  }

  case Action::Type::TakeOut:
    for (ThingRef thing : get_things())
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_take_out(thing, action_time);
        if (success)
        {
          actor->add_to_property<int>("counter_busy", action_time);
        }
      }
    }
    break;

  case Action::Type::Use:
    for (ThingRef thing : get_things())
    {
      if (thing != ThingManager::get_mu())
      {
        success = actor->do_use(thing, action_time);
        if (success)
        {
          actor->add_to_property<int>("counter_busy", action_time);
        }
      }
    }
    break;

  case Action::Type::Wield:
  {
    if (number_of_things > 1)
    {
      the_message_log.add("NOTE: Only wielding the last item selected.");
    }

    ThingRef thing = get_things()[number_of_things - 1];
    if (thing != ThingManager::get_mu())
    {
      /// @todo Implement wielding using other hands.
      success = actor->do_wield(thing, 0, action_time);
      if (success)
      {
        actor->add_to_property<int>("counter_busy", action_time);
      }
    }
    break;
  }

  default:
    the_message_log.add("We're sorry, but that action has not yet been implemented.");
    break;
  } // end switch (action)

  return success;
}

void Action::finish_(ThingRef actor, AnyMap params)
{
  /// @todo Complete the action here, and set counter_busy to how long
  ///       of a delay occurs post-finish.
}

void Action::abort_(ThingRef actor, AnyMap params)
{
}

void Action::set_state(Action::State state)
{
  m_state = state;
}

Action::State Action::get_state()
{
  return m_state;
}


// @todo Action::target_can_be_bodypart()

bool Action::target_can_be_thing() const
{
  switch (m_type)
  {
  case Action::Type::Fill:
  case Action::Type::PutInto:
    return true;

  default:
    return false;
  }
}

bool Action::target_can_be_direction() const
{
  switch (m_type)
  {
  case Action::Type::Attack:    
  case Action::Type::Close:
  case Action::Type::Fill:
  case Action::Type::Hurl:
  case Action::Type::Move:
  case Action::Type::Open:
  case Action::Type::Shoot:
    return true;

  default:
    return false;
  }
}

void Action::set_target(ThingRef target)
{
  m_target_thing = target;
  m_target_direction = Direction::None;
}

void Action::set_target(Direction direction)
{
  m_target_thing = ThingRef();
  m_target_direction = direction;
}

ThingRef const& Action::get_target_thing() const
{
  return m_target_thing;
}

Direction const& Action::get_target_direction() const
{
  return m_target_direction;
}

unsigned int Action::get_quantity() const
{
  return m_quantity;
}

void Action::set_quantity(unsigned int quantity)
{
  m_quantity = quantity;
}
