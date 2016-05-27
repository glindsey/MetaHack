#include "stdafx.h"

#include "ActionGet.h"
#include "ActionMove.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionGet, "get", L"get")

Action::StateResult ActionGet::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();
  ThingId location = subject->get_location();

  // Verify that the Action has an object.
  if (object == ThingId::Mu())
  {
    return StateResult::Failure();
  }

  // Check that it isn't US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      message = L"Oh, ha ha, I get it, \"pick me up\".  Nice try.";
    }
    else
    {
      message = YOU_TRY + L" to pick " + YOURSELF +
        L"up, which seriously shouldn't happen.";
      CLOG(WARNING, "Action") << "NPC tried to pick self up!?";
    }
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Check if it's already in our inventory.
  if (subject->get_inventory().contains(object))
  {
    message = YOU_TRY + L" to pick up " + THE_FOO + L".";
    the_message_log.add(message);

    message = THE_FOO + FOO_IS + L" already in " + YOUR + L" inventory!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  /// @todo When picking up, check if our inventory is full-up.
  if (false)
  {
    message = YOU_TRY + L" to pick up " + THE_FOO + L".";
    the_message_log.add(message);

    message = YOUR + L" inventory cannot accommodate " + THE_FOO + L".";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  if (!object->can_have_action_done_by(subject, ActionMove::prototype))
  {
    message = YOU_TRY + L" to pick up " + THE_FOO + L".";
    the_message_log.add(message);

    message = YOU + L" cannot move " + THE_FOO + L".";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionGet::do_begin_work_(AnyMap& params)
{
  /// @todo Handle getting a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  if (object->be_object_of(*this, subject) == ActionResult::Success)
  {
    message = YOU + CV(L" pick", L" picks") + L" up " + THE_FOO + L".";
    the_message_log.add(message);
    if (object->move_into(subject))
    {
      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else // could not add to inventory
    {
      message = YOU + L" could not pick up " + THE_FOO + L" for some inexplicable reason.";
      the_message_log.add(message);

      CLOG(WARNING, "Action") << "Could not move Thing " << object <<
        " even though be_object_of returned Success";
    }
  }

  return result;
}

Action::StateResult ActionGet::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionGet::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}