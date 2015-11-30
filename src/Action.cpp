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
  if (actor->get_property<int>("counter_busy") > 0)
  {
    actor->add_to_property<int>("counter_busy", -1);
    return true;
  }

  // Perform any type-specific processing.
  // Useful if, for example, your Entity can rise from the dead.
  /// @todo Figure out how to implement this safely.
  //actor->call_lua_function("process", {});

  // Continue running through states until the event is processed, or the
  // target actor is busy.
  while ((m_state != Action::State::Processed) && (actor->get_property<int>("counter_busy") == 0))
  {
    switch (m_state)
    {
    case Action::State::Pending:
      prebegin_(actor, params);
      break;

    case Action::State::PreBegin:
      begin_(actor, params);
      break;

    case Action::State::InProgress:
      finish_(actor, params);
      break;

    case Action::State::Interrupted:
      abort_(actor, params);
      break;

    case Action::State::PostFinish:
      set_state(Action::State::Processed);
      break;

    default:
      break;
    }
  }

  return true;
}

bool Action::prebegin_(ThingRef actor, AnyMap& params)
{
  /// @todo Set counter_busy based on the action being taken and
  ///       the entity's reflexes.
  actor->set_property<int>("counter_busy", 0);
  set_state(Action::State::PreBegin);
  return true;
}

bool Action::begin_(ThingRef actor, AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  ThingRef thing = ThingManager::get_mu();

  if (m_things.size() > 0)
  {
    thing = m_things.back();
  }

  switch (get_type())
  {
  case Action::Type::Wait:
    success = actor->do_move(Direction::Self, action_time);
    break;

  case Action::Type::Move:
    success = actor->do_move(get_target_direction(), action_time);
    break;

  case Action::Type::Attack:
    success = actor->do_attack(get_target_direction(), action_time);
    break;

  case Action::Type::Drop:
    if (thing != ThingManager::get_mu())
    {
      success = actor->do_drop(thing, action_time);
    }
    break;

  case Action::Type::Eat:
    if (thing != ThingManager::get_mu())
    {
      success = actor->do_eat(thing, action_time);
    }
    break;

  case Action::Type::Get:
    if (thing != ThingManager::get_mu())
    {
      success = actor->do_pick_up(thing, action_time);
    }
    break;

  case Action::Type::Quaff:
    if (thing != ThingManager::get_mu())
    {
      success = actor->do_drink(thing, action_time);
    }
    break;

  case Action::Type::PutInto:
  {
    ThingRef container = get_target_thing();
    if (container != ThingManager::get_mu())
    {
      if (container->get_intrinsic<int>("inventory_size") != 0)
      {
        if (thing != ThingManager::get_mu())
        {
          success = actor->do_put_into(thing, container, action_time);
        }
        break;
      }
    }
    else
    {
      the_message_log.add("That target is not a container.");
    }
    break;
  }

  case Action::Type::TakeOut:
    if (thing != ThingManager::get_mu())
    {
      success = actor->do_take_out(thing, action_time);
    }
    break;

  case Action::Type::Use:
    if (thing != ThingManager::get_mu())
    {
      success = actor->do_use(thing, action_time);
    }
    break;

  case Action::Type::Wield:
  {
    if (thing != ThingManager::get_mu())
    {
      /// @todo Implement wielding using other hands.
      success = actor->do_wield(thing, 0, action_time);
    }
    break;
  }

  default:
    the_message_log.add("We're sorry, but that action has not yet been implemented.");
    break;
  } // end switch (action)

  // If starting the action succeeded, move to the in-progress state.
  // Otherwise, just go right to post-finish.
  if (success)
  {
    // Update the busy counter.
    actor->set_property<int>("counter_busy", action_time);
    set_state(Action::State::InProgress);
  }
  else
  {
    // Clear the busy counter.
    actor->set_property<int>("counter_busy", 0);
    set_state(Action::State::PostFinish);
  }

  return success;
}

void Action::finish_(ThingRef actor, AnyMap& params)
{
  unsigned int action_time = 0;

  /// @todo Complete the action here

  // If there are any things in the m_things queue, pop the back one off.
  if (m_things.size() > 0)
  {
    m_things.pop_back();
  }

  // If there are still things left, go back to the pre-begin state to handle
  // the next thing; otherwise, move to the post-finish state.
  if (m_things.size() > 0)
  {
    set_state(Action::State::PreBegin);
  }
  else
  {
    actor->set_property<int>("counter_busy", action_time);
    set_state(Action::State::PostFinish);
  }
}

void Action::abort_(ThingRef actor, AnyMap& params)
{
  unsigned int action_time = 0;

  /// @todo Handle aborting the action here.

  actor->add_to_property<int>("counter_busy", action_time);
  set_state(Action::State::PostFinish);
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