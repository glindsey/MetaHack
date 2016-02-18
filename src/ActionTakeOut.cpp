#include "ActionTakeOut.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_BOILERPLATE(ActionTakeOut)

Action::StateResult ActionTakeOut::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();

  // Verify that the Action has an object.
  if (object == ThingManager::get_mu())
  {
    return StateResult::Failure();
  }

  // Check that the thing isn't US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      /// @todo Maybe allow player to voluntarily exit a container?
      message = "I'm afraid you can't do that.  "
        "(At least, not in this version...)";
    }
    else
    {
      message = YOU_TRY + " to take " + YOURSELF +
        "out, which seriously shouldn't happen.";
      MINOR_ERROR("NPC tried to take self out!?");
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is not a MapTile or Entity.
  if (!object->is_inside_another_thing())
  {
    message = YOU_TRY + " to remove " + THE_FOO +
      " from its container.";
    the_message_log.add(message);

    message = "But " + THE_FOO + " is not inside a container!";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is within reach.
  if (!subject->can_reach(container))
  {
    message = YOU_TRY + " to remove " + THE_FOO + " from " +
      THE_FOOS_LOCATION + ".";
    the_message_log.add(message);

    message = YOU + " cannot reach " + THE_FOO + ".";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionTakeOut::do_begin_work_(AnyMap& params)
{
  /// @todo Handle taking out a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();
  auto new_location = container->get_location();

  if (object->perform_action_taken_out_by(subject))
  {
    if (object->move_into(new_location))
    {
      message = YOU + CV(" remove ", "removes ") +
        THE_FOO + " from " +
        THE_FOOS_LOCATION + ".";
      the_message_log.add(message);

      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else
    {
      message = YOU + " could not take " + THE_FOO + " out of " + THE_FOOS_LOCATION + " for some inexplicable reason.";
      the_message_log.add(message);

      MAJOR_ERROR("Could not move Thing out of Container");
    }
  }

  return result;
}

Action::StateResult ActionTakeOut::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionTakeOut::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}