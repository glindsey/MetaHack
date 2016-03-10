#include "stdafx.h"

#include "ActionRead.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionRead, "read", "read")

Action::StateResult ActionRead::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  if (false) ///< @todo Intelligence tests for reading.
  {
    print_message_try_();

    message = YOU_ARE + " not smart enough to read " + FOOSELF + ".";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionRead::do_begin_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  /// @todo Figure out read time.
  print_message_begin_();
  result = StateResult::Success(1);

  return result;
}

Action::StateResult ActionRead::do_finish_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  print_message_finish_();

  /// @todo Split read time into start/finish actions.
  switch (object->be_object_of(*this, subject))
  {
    case ActionResult::SuccessDestroyed:
      message = THE_FOO + OBJCV(" disintegrate", " disintegrates") + " after " + YOU + CV(" read ",
                                                                                          " reads ") + OBJ_PRO_FOO + "!";
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
}

Action::StateResult ActionRead::do_abort_work_(AnyMap& params)
{
  auto subject = get_subject();
  auto object = get_object();

  print_message_stop_();
  return Action::StateResult::Success();
}

void ActionRead::print_message_cant_()
{
  StringDisplay message = THE_FOO + FOO_HAS + " no writing to read.";
  the_message_log.add(message);
}