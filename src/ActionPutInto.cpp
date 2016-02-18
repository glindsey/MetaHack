#include "ActionPutInto.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionPutInto, "PutInto")

Action::StateResult ActionPutInto::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = get_target_thing();

  // Verify that the Action has an object.
  if (object == ThingManager::get_mu())
  {
    return StateResult::Failure();
  }

  // Check that the thing and container aren't the same thing.
  if (object == container)
  {
    if (IS_PLAYER)
    {
      message = "That would be an interesting topological exercise.";
    }
    else
    {
      message = YOU_TRY + " to store " + THE_FOO +
        "inside itself, which seriously shouldn't happen.";
      MINOR_ERROR("NPC tried to store a container in itself!?");
    }

    return StateResult::Failure();
  }

  // Check that the thing is not US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      /// @todo Possibly allow player to voluntarily enter a container?
      message = "I'm afraid you can't do that.  "
        "(At least, not in this version...)";
    }
    else
    {
      message = YOU_TRY + " to store " + YOURSELF +
        "into " + THE_TARGET_THING +
        ", which seriously shouldn't happen.";
      MINOR_ERROR("NPC tried to store self!?");
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is not US!
  if (container == subject)
  {
    if (IS_PLAYER)
    {
      message = "Store something in yourself?  What do you think you are, a drug mule?";
    }
    else
    {
      message = YOU_TRY + " to store " + THE_FOO + "into " + YOURSELF +
        ", which seriously shouldn't happen.";
      MINOR_ERROR("NPC tried to store into self!?");
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container actually IS a container.
  if (container->get_intrinsic<int>("inventory_size") == 0)
  {
    message = YOU_TRY + " to store " + THE_FOO + " in " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);

    message = THE_TARGET_THING + " is not a container!";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the thing's location isn't already the container.
  if (object->get_location() == container)
  {
    message = YOU_TRY + " to store " + THE_FOO + " in " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);

    message = THE_FOO + " is already in " +
      THE_TARGET_THING + "!";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the thing is within reach.
  if (!subject->can_reach(object))
  {
    message = YOU_TRY + " to store " + THE_FOO + " in " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);

    message = YOU + " cannot reach " + THE_FOO;

    if (!subject->can_reach(container))
    {
      message += " (or " + THE_TARGET_THING + ")";
    }

    message += ".";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is within reach.
  if (!subject->can_reach(container))
  {
    message = YOU_TRY + " to store " + THE_FOO + " in " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);

    message = YOU + " cannot reach " + THE_TARGET_THING + ".";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that we're not wielding the item.
  if (subject->is_wielding(object))
  {
    message = YOU_TRY + " to store " + THE_FOO + " in " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);

    message = YOU + " cannot store something that is currently being worn.";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  /// @todo Check that we're not wearing the item.
  if (subject->has_equipped(object))
  {
    message = YOU_TRY + " to store " + THE_FOO + " in " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);

    /// @todo Perhaps automatically try to unwield the item before dropping?
    message = YOU + "cannot store something that is currently being wielded.";
    the_message_log.add(message);

    return StateResult::Failure(); //ActionResult::FailureItemEquipped;
  }

  return StateResult::Success();
}

Action::StateResult ActionPutInto::do_begin_work_(AnyMap& params)
{
  /// @todo Handle putting a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = get_target_thing();

  if (object->perform_action_put_into_by(container, subject))
  {
    message = YOU + CV(" place ", "places ") +
      THE_FOO + " into " +
      THE_TARGET_THING + ".";
    the_message_log.add(message);
    if (object->move_into(container))
    {
      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else
    {
      message = YOU + " could not move " + THE_FOO + " into " + THE_TARGET_THING + " for some inexplicable reason.";
      the_message_log.add(message);

      MAJOR_ERROR("Could not move Thing into Container");
    }
  }

  return result;
}

Action::StateResult ActionPutInto::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionPutInto::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}