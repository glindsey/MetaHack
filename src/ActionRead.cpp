#include "ActionRead.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionRead, "Read", "read")

Action::StateResult ActionRead::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  // Check that the thing is within reach.
  if (!subject->can_reach(object))
  {
    print_message_try_();

    message = "However, " + OBJ_PRO_FOO + " is out of " + YOUR + " reach.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

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
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  if (object->is_readable_by(subject))
  {
    /// @todo Figure out read time.
    message = YOU + CV(" begin", " begins") + " to read " + FOOSELF + ".";
    the_message_log.add(message);

    result = StateResult::Success(1);
  }
  else
  {
    message = YOU_TRY + " to read " + FOOSELF + ".";
    the_message_log.add(message);

    message = THE_FOO + FOO_HAS + " no writing to read.";
    the_message_log.add(message);
  }

  return result;
}

Action::StateResult ActionRead::do_finish_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  message = YOU + CV(" finish", " finishes") + " reading " + FOOSELF + ".";
  the_message_log.add(message);

  /// @todo Split read time into start/finish actions.
  switch (object->perform_action_read_by(subject))
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
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  message = YOU + CV(" stop", " stops") + " reading " + FOOSELF + ".";
  the_message_log.add(message);

  return Action::StateResult::Success();
}