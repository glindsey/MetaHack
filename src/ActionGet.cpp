#include "ActionGet.h"
#include "Thing.h"
#include "ThingRef.h"

ActionGet::ActionGet(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionGet::ActionGet(ThingRef subject, ThingRef object, unsigned int quantity)
  :
  Action(subject, object, quantity)
{}

ActionGet::~ActionGet()
{}

Action::StateResult ActionGet::do_prebegin_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  ThingRef location = subject->get_location();

  // Verify that the Action has an object.
  if (object == ThingManager::get_mu())
  {
    return StateResult::Failure();
  }

  // Check that it isn't US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      message = "Oh, ha ha, I get it, \"pick me up\".  Nice try.";
    }
    else
    {
      message = YOU_TRY + " to pick " + YOURSELF +
        "up, which seriously shouldn't happen.";
      MINOR_ERROR("NPC tried to pick self up!?");
    }
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Check if it's already in our inventory.
  if (subject->get_inventory().contains(object))
  {
    message = YOU_TRY + " to pick up " + THE_FOO + ".";
    the_message_log.add(message);

    message = THE_FOO + FOO_IS + " already in " + YOUR + " inventory!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Check that it's within reach.
  if (!subject->can_reach(object))
  {
    message = YOU_TRY + " to pick up " + THE_FOO + ".";
    the_message_log.add(message);

    message = THE_FOO + FOO_IS + " out of " + YOUR + " reach.";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  /// @todo When picking up, check if our inventory is full-up.
  if (false)
  {
    message = YOU_TRY + " to pick up " + THE_FOO + ".";
    the_message_log.add(message);

    message = YOUR + " inventory cannot accommodate " + THE_FOO + ".";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  if (!object->is_movable_by(subject))
  {
    message = YOU_TRY + " to pick up " + THE_FOO + ".";
    the_message_log.add(message);

    message = YOU + " cannot move " + THE_FOO + ".";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionGet::do_begin_work(AnyMap& params)
{
  /// @todo Handle getting a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  if (object->perform_action_picked_up_by(subject))
  {
    message = YOU + CV(" pick", " picks") + " up " + THE_FOO + ".";
    the_message_log.add(message);
    if (object->move_into(subject))
    {
      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else // could not add to inventory
    {
      message = YOU + " could not pick up " + THE_FOO + " for some inexplicable reason.";
      the_message_log.add(message);

      MAJOR_ERROR("Could not move Thing even though "
                  "is_movable returned Success");
    }
  }

  return result;
}

Action::StateResult ActionGet::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionGet::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}