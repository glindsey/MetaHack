#include "stdafx.h"

#include "ActionTakeOut.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionTakeOut, "takeout", L"remove")

Action::StateResult ActionTakeOut::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();

  // Verify that the Action has an object.
  if (object == ThingId::Mu())
  {
    return StateResult::Failure();
  }

  // Check that the thing isn't US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      /// @todo Maybe allow player to voluntarily exit a container?
      message = L"I'm afraid you can't do that.  "
        L"(At least, not in this version...)";
    }
    else
    {
      message = YOU_TRY + L" to take " + YOURSELF +
        L"out, which seriously shouldn't happen.";
      CLOG(WARNING, "Action") << "NPC tried to take self out!?";
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is not a MapTile or Entity.
  if (!object->is_inside_another_thing())
  {
    print_message_try_();

    //message = YOU_TRY + " to remove " + THE_FOO +
    //  " from its container.";
    //the_message_log.add(message);

    message = L"But " + THE_FOO + L" is not inside a container!";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is within reach.
  if (!subject->can_reach(container))
  {
    print_message_try_();

    message = YOU + L" cannot reach " + THE_FOO + L".";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionTakeOut::do_begin_work_(AnyMap& params)
{
  /// @todo Handle taking out a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();
  auto new_location = container->get_location();

  // Set the target to be the container as a kludge for message printing.
  set_target(container);

  if (object->be_object_of(*this, subject) == ActionResult::Success)
  {
    if (object->move_into(new_location))
    {
      print_message_do_();

      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else
    {
      message = YOU + L" could not take " + get_object_string_() + L" out of " + get_target_string_() + L" for some inexplicable reason.";
      the_message_log.add(message);

      MAJOR_ERROR("Could not move Thing out of Container even though be_object_of returned Success");
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

void ActionTakeOut::print_message_try_() const
{
  StringDisplay message = YOU_TRY + L" to " + VERB + L" " + get_object_string_() + L" from " + get_target_string_() + L".";
  the_message_log.add(message);
}

void ActionTakeOut::print_message_do_() const
{
  StringDisplay message = YOU + L" " + CV(VERB, VERB3) + L" " + get_object_string_() + L" from " + get_target_string_() + L".";
  the_message_log.add(message);
}