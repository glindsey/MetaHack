#include "ActionRead.h"
#include "Thing.h"
#include "ThingRef.h"

ActionRead::ActionRead(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionRead::~ActionRead()
{}

Action::StateResult ActionRead::do_prebegin_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  // Check that the thing is within reach.
  if (!subject->can_reach(object))
  {
    message = YOU_TRY_TO("read") + FOO + ", but it is out of " + YOUR + " reach.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  if (false) ///< @todo Intelligence tests for reading.
  {
    message = YOU_TRY + " to read " + FOOSELF + ".";
    the_message_log.add(message);

    message = YOU_ARE + " not smart enough to read " + FOOSELF + ".";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionRead::do_begin_work(AnyMap& params)
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

    message = FOO + OBJCV(" have", " has") + " no writing to read.";
    the_message_log.add(message);
  }

  return result;
}

Action::StateResult ActionRead::do_finish_work(AnyMap& params)
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
      message = FOO + " disintegrates after " + YOU + CV(" read", " reads") + " it.";
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

Action::StateResult ActionRead::do_abort_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  message = YOU + CV(" stop", " stops") + " reading " + FOOSELF + ".";
  the_message_log.add(message);

  return Action::StateResult::Success();
}