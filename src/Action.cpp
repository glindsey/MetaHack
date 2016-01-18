#include "Action.h"

#include <algorithm>

#include "MessageLog.h"
#include "Thing.h"
#include "ThingManager.h"
#include "ThingRef.h"

struct Action::Impl
{
  Impl(Action::Type type_)
    :
    type{ type_ },
    state{ Action::State::Pending },
    things{ std::vector<ThingRef>{ } },
    target_thing{ ThingManager::get_mu() },
    target_direction{ Direction::None },
    quantity{ 0 }
  {}

  /// Type of this action.
  Action::Type type;

  /// State of this action.
  Action::State state;

  /// Thing(s) to perform the action on.
  std::vector<ThingRef> things;

  /// Target Thing for the action (if any).
  ThingRef target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

Action::Action(Action::Type type)
  :
  pImpl{ new Impl(type) }
{}

Action::~Action()
{}

Action::Action(Action const& other)
  :
  pImpl{ new Impl(*(other.pImpl.get())) }
{}

Action::Action(Action&& other)
  :
  pImpl{ std::move(other.pImpl) }
{}

Action& Action::operator=(Action other)
{
  std::swap(pImpl, other.pImpl);
  return *this;
}

Action::Type Action::get_type() const
{
  return pImpl->type;
}

void Action::set_things(std::vector<ThingRef> things)
{
  pImpl->things = things;
}

std::vector<ThingRef> const& Action::get_things() const
{
  return pImpl->things;
}

void Action::add_thing(ThingRef thing)
{
  pImpl->things.push_back(thing);
}

bool Action::remove_thing(ThingRef thing)
{
  auto& iter = std::find(pImpl->things.begin(), pImpl->things.end(), thing);
  if (iter != pImpl->things.end())
  {
    pImpl->things.erase(iter);
    return true;
  }
  return false;
}

void Action::clear_things()
{
  pImpl->things.clear();
}

bool Action::process(ThingRef actor, AnyMap params)
{
  // If entity is currently busy, decrement by one and return.
  if (actor->get_property<int>("counter_busy") > 0)
  {
    actor->add_to_property<int>("counter_busy", -1);
    return false;
  }

  // Perform any type-specific processing.
  // Useful if, for example, your Entity can rise from the dead.
  /// @todo Figure out how to implement this safely.
  //actor->call_lua_function("process", {});

  // Continue running through states until the event is processed, or the
  // target actor is busy.
  while ((pImpl->state != Action::State::Processed) && (actor->get_property<int>("counter_busy") == 0))
  {
    switch (pImpl->state)
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
  bool success = false;
  unsigned int action_time = 0;

  success = do_prebegin_work(actor, params, action_time);
  actor->set_property<int>("counter_busy", 0);

  set_state(Action::State::PreBegin);
  return true;
}

bool Action::begin_(ThingRef actor, AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  ThingRef thing = ThingManager::get_mu();

  if (pImpl->things.size() > 0)
  {
    thing = pImpl->things.back();
  }

  success = do_begin_work(actor, thing, params, action_time);

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

  do_finish_work(actor, params, action_time);

  // If there are any things in the m_things queue, pop the back one off.
  if (pImpl->things.size() > 0)
  {
    pImpl->things.pop_back();
  }

  // If there are still things left, go back to the pre-begin state to handle
  // the next thing; otherwise, move to the post-finish state.
  if (pImpl->things.size() > 0)
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

  do_abort_work(actor, params, action_time);

  actor->add_to_property<int>("counter_busy", action_time);
  set_state(Action::State::PostFinish);
}

void Action::set_state(Action::State state)
{
  pImpl->state = state;
}

Action::State Action::get_state()
{
  return pImpl->state;
}

void Action::set_target(ThingRef target)
{
  pImpl->target_thing = target;
  pImpl->target_direction = Direction::None;
}

void Action::set_target(Direction direction)
{
  pImpl->target_thing = ThingRef();
  pImpl->target_direction = direction;
}

ThingRef const& Action::get_target_thing() const
{
  return pImpl->target_thing;
}

Direction const& Action::get_target_direction() const
{
  return pImpl->target_direction;
}

unsigned int Action::get_quantity() const
{
  return pImpl->quantity;
}

void Action::set_quantity(unsigned int quantity)
{
  pImpl->quantity = quantity;
}

// @todo Action::target_can_be_bodypart()

bool Action::target_can_be_thing() const
{
  switch (pImpl->type)
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
  switch (pImpl->type)
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

bool Action::do_prebegin_work(ThingRef actor, AnyMap& params, unsigned int& action_time)
{
  /// @todo Set counter_busy based on the action being taken and
  ///       the entity's reflexes.
  action_time = 0;
  return true;
}

bool Action::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params, unsigned int& action_time)
{
  bool success;

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

  return success;
}

void Action::do_finish_work(ThingRef actor, AnyMap& params, unsigned int& action_time)
{
  /// @todo Complete the action here
  action_time = 0;
}

void Action::do_abort_work(ThingRef actor, AnyMap& params, unsigned int& action_time)
{
  /// @todo Handle aborting the action here.
  action_time = 0;
}