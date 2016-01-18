#include "Action.h"

#include <algorithm>

#include "MessageLog.h"
#include "Thing.h"
#include "ThingManager.h"
#include "ThingRef.h"

struct Action::Impl
{
  Impl()
    :
    state{ Action::State::Pending },
    things{ std::vector<ThingRef>{ } },
    target_thing{ ThingManager::get_mu() },
    target_direction{ Direction::None },
    quantity{ 0 }
  {}

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

Action::Action()
  :
  pImpl{ new Impl() }
{}

Action::~Action()
{}

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

bool Action::target_can_be_thing() const
{
  return false;
}

bool Action::target_can_be_direction() const
{
  return false;
}

bool Action::prebegin_(ThingRef actor, AnyMap& params)
{
  auto result = do_prebegin_work(actor, params);

  if (result.success)
  {
    actor->set_property<int>("counter_busy", result.elapsed_time);
  }

  set_state(Action::State::PreBegin);
  return true;
}

bool Action::begin_(ThingRef actor, AnyMap& params)
{
  ThingRef thing = ThingManager::get_mu();

  if (pImpl->things.size() > 0)
  {
    thing = pImpl->things.back();
  }

  auto result = do_begin_work(actor, thing, params);

  // If starting the action succeeded, move to the in-progress state.
  // Otherwise, just go right to post-finish.
  if (result.success)
  {
    // Update the busy counter.
    actor->set_property<int>("counter_busy", result.elapsed_time);
    set_state(Action::State::InProgress);
  }
  else
  {
    // Clear the busy counter.
    actor->set_property<int>("counter_busy", 0);
    set_state(Action::State::PostFinish);
  }

  return result.success;
}

void Action::finish_(ThingRef actor, AnyMap& params)
{
  auto result = do_finish_work(actor, params);

  // If there are any things in the things queue, pop the back one off.
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
    if (result.success)
    {
      actor->set_property<int>("counter_busy", result.elapsed_time);
    }
    set_state(Action::State::PostFinish);
  }
}

void Action::abort_(ThingRef actor, AnyMap& params)
{
  auto result = do_abort_work(actor, params);

  if (result.success)
  {
    actor->add_to_property<int>("counter_busy", result.elapsed_time);
  }
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

Action::StateResult Action::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  /// @todo Set counter_busy based on the action being taken and
  ///       the entity's reflexes.
  return{ true, 0 };
}

Action::StateResult Action::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult Action::do_finish_work(ThingRef actor, AnyMap& params)
{
  /// @todo Complete the action here
  return{ true, 0 };
}

Action::StateResult Action::do_abort_work(ThingRef actor, AnyMap& params)
{
  /// @todo Handle aborting the action here.
  return{ true, 0 };
}