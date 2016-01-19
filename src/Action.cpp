#include "Action.h"

#include <algorithm>

#include "MessageLog.h"
#include "Thing.h"
#include "ThingManager.h"
#include "ThingRef.h"

struct Action::Impl
{
  Impl(ThingRef subject_,
       std::vector<ThingRef> objects_,
       unsigned int quantity_)
    :
    state{ Action::State::Pending },
    subject{ subject_ },
    objects{ objects_ },
    target_thing{ ThingManager::get_mu() },
    target_direction{ Direction::None },
    quantity{ quantity_ }
  {}

  /// State of this action.
  Action::State state;

  /// The subject performing the action.
  ThingRef subject;

  /// The objects of the action.
  std::vector<ThingRef> objects;

  /// Target Thing for the action (if any).
  ThingRef target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

Action::Action(ThingRef subject)
  :
  pImpl{ new Impl(subject, {}, 0) }
{}

Action::Action(ThingRef subject, ThingRef object)
  :
  pImpl{ new Impl(subject, { object }, 1) }
{}

Action::Action(ThingRef subject, ThingRef object, unsigned int quantity)
  :
  pImpl{ new Impl(subject,{object}, quantity) }
{}

Action::Action(ThingRef subject, std::vector<ThingRef> objects)
  :
  pImpl{ new Impl(subject, objects, 1) }
{}

Action::~Action()
{}

ThingRef Action::get_subject() const
{
  return pImpl->subject;
}

std::vector<ThingRef> const& Action::get_objects() const
{
  return pImpl->objects;
}

bool Action::process(ThingRef actor, AnyMap params)
{
  // If entity is currently busy, decrement by one and return.
  unsigned int counter_busy = actor->get_property<int>("counter_busy");
  if (counter_busy > 0)
  {
    TRACE("Thing #%s (%s): counter_busy = %d, decrementing",
          actor.get_id().to_string().c_str(),
          actor->get_type().c_str(),
          counter_busy);
    actor->add_to_property<int>("counter_busy", -1);
    return false;
  }

  // Perform any type-specific processing.
  // Useful if, for example, your Entity can rise from the dead.
  /// @todo Figure out how to implement this safely.
  //actor->call_lua_function("process", {});

  // Continue running through states until the event is processed, or the
  // target actor is busy.
  while ((pImpl->state != Action::State::Processed) && (counter_busy == 0))
  {
    unsigned int counter_busy = actor->get_property<int>("counter_busy");

    TRACE("Thing #%s (%s): Action %s is in state %s, counter_busy = %d",
          actor.get_id().to_string().c_str(),
          actor->get_type().c_str(),
          get_type().c_str(),
          Action::str(get_state()),
          counter_busy);

    switch (pImpl->state)
    {
      case Action::State::Pending:
        prebegin_(params);
        break;

      case Action::State::PreBegin:
        begin_(params);
        break;

      case Action::State::InProgress:
        finish_(params);
        break;

      case Action::State::Interrupted:
        abort_(params);
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

/// @todo Action::target_can_be_bodypart()

bool Action::prebegin_(AnyMap& params)
{
  auto result = do_prebegin_work(params);

  if (result.success)
  {
    // Update the busy counter.
    pImpl->subject->set_property<int>("counter_busy", result.elapsed_time);
    set_state(Action::State::PreBegin);
  }
  else
  {
    // Clear the busy counter.
    pImpl->subject->set_property<int>("counter_busy", 0);
    set_state(Action::State::PostFinish);
  }

  return true;
}

bool Action::begin_(AnyMap& params)
{
  auto result = do_begin_work(params);

  // If starting the action succeeded, move to the in-progress state.
  // Otherwise, just go right to post-finish.
  if (result.success)
  {
    // Update the busy counter.
    pImpl->subject->set_property<int>("counter_busy", result.elapsed_time);
    set_state(Action::State::InProgress);
  }
  else
  {
    // Clear the busy counter.
    pImpl->subject->set_property<int>("counter_busy", 0);
    set_state(Action::State::PostFinish);
  }

  return result.success;
}

void Action::finish_(AnyMap& params)
{
  auto result = do_finish_work(params);

  pImpl->subject->set_property<int>("counter_busy", result.elapsed_time);
  set_state(Action::State::PostFinish);
}

void Action::abort_(AnyMap& params)
{
  auto result = do_abort_work(params);

  pImpl->subject->add_to_property<int>("counter_busy", result.elapsed_time);
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

void Action::set_target(ThingRef thing) const
{
  pImpl->target_thing = thing;
  pImpl->target_direction = Direction::None;
}

void Action::set_target(Direction direction) const
{
  pImpl->target_thing = ThingManager::get_mu();
  pImpl->target_direction = direction;
}

void Action::set_quantity(unsigned int quantity) const
{
  pImpl->quantity = quantity;
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

Action::StateResult Action::do_prebegin_work(AnyMap& params)
{
  /// @todo Set counter_busy based on the action being taken and
  ///       the entity's reflexes.
  return{ true, 0 };
}

Action::StateResult Action::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult Action::do_finish_work(AnyMap& params)
{
  /// @todo Complete the action here
  return{ true, 0 };
}

Action::StateResult Action::do_abort_work(AnyMap& params)
{
  /// @todo Handle aborting the action here.
  return{ true, 0 };
}