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
  MapTile* current_tile = subject->get_maptile();
  Direction new_direction = get_target_direction();

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
      MINOR_ERROR("Non-player Entity tried to pick self up!?");
    }
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Check if it's already in our inventory.
  if (subject->get_inventory().contains(object))
  {
    message = YOU_TRY + " to pick up " + object->get_identifying_string();
    the_message_log.add(message);

    message = object->get_identifying_string() + " is already in " +
      YOUR + " inventory!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Check that it's within reach.
  if (!subject->can_reach(object))
  {
    message = YOU_TRY + " to pick up " + object->get_identifying_string();
    the_message_log.add(message);

    message = object->get_identifying_string() + " is out of " + YOUR + " reach.";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  /// @todo When picking up, check if our inventory is full-up.
  if (false)
  {
    message = YOU_TRY + " to pick up " + object->get_identifying_string();
    the_message_log.add(message);

    message = YOUR + " inventory cannot accomodate " + object->get_identifying_string();
    the_message_log.add(message);
    return StateResult::Failure();
  }

  if (!object->is_movable_by(subject))
  {
    message = YOU_TRY + " to pick up " + object->get_identifying_string();
    the_message_log.add(message);

    message = YOU + " cannot move the " + object->get_identifying_string() + ".";
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
    message = YOU + CV(" pick", " picks") + " up " + FOO + ".";
    the_message_log.add(message);
    if (object->move_into(subject))
    {
      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else // could not add to inventory
    {
      MAJOR_ERROR("Could not move Thing even though "
                  "is_movable returned Success");
    }
  }
  else // perform_action_picked_up_by(pImpl->id) returned false
  {
    // perform_action_picked_up_by() will print any relevant messages
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