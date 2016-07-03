#include "stdafx.h"

#include "ActionRead.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionRead, "read", L"read")

Action::StateResult ActionRead::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  if (false) ///< @todo Intelligence tests for reading.
  {
    print_message_try_();

    message = YOU_ARE + L" not smart enough to read " + FOOSELF + L".";
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
      message = THE_FOO + OBJCV(L" disintegrate", L" disintegrates") + L" after " + YOU + CV(L" read ", L" reads ") + OBJ_PRO_FOO + L"!";
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

void ActionRead::print_message_cant_() const
{
  StringDisplay message = THE_FOO + FOO_HAS + L" no writing to read.";
  the_message_log.add(message);
}