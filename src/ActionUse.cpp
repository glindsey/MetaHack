#include "stdafx.h"

#include "ActionUse.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionUse, "use", "use")

Action::StateResult ActionUse::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  // Check that it is not us!
  if (subject == object)
  {
    message = YOU_TRY + " to use " + THE_FOO + ".";
    the_message_log.add(message);

    if (IS_PLAYER)
    {
      message = YOU_ARE + " already using " + YOURSELF + " to the best of " + YOUR + " ability.";
      the_message_log.add(message);
    }
    else
    {
      message = "That seriously shouldn't happen!";
      the_message_log.add(message);

      MINOR_ERROR("NPC tried to use self!?");
    }

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionUse::do_begin_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  /// @todo Figure out use time.
  print_message_begin_();
  result = StateResult::Success(1);

  return result;
}

Action::StateResult ActionUse::do_finish_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  print_message_finish_();

  /// @todo Split read time into start/finish actions.
  switch (object->be_object_of(*this, subject))
  {
    case ActionResult::SuccessDestroyed:
      message = THE_FOO + OBJCV(" disintegrate", " disintegrates") + " after " + YOU + CV(" use ",
                                                                                          " uses ") + OBJ_PRO_FOO + "!";
      the_message_log.add(message);

      object->destroy();
      result = StateResult::Success();
      break;

    case ActionResult::Success:
      result = StateResult::Success();
      break;

    case ActionResult::Failure:
    default:
      result = StateResult::Failure();
      break;
  }

  return result;

  return Action::StateResult::Success();
}

Action::StateResult ActionUse::do_abort_work_(AnyMap& params)
{
  auto subject = get_subject();
  auto object = get_object();

  print_message_stop_();

  return Action::StateResult::Success();
}