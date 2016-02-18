#include "ActionWait.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionWait, "Wait")

Action::StateResult ActionWait::do_prebegin_work_(AnyMap& params)
{
  // We can always wait.
  return Action::StateResult::Success();
}

Action::StateResult ActionWait::do_begin_work_(AnyMap& params)
{
  std::string message =
    YOU + " successfully" + CV(" stay", " stays") +
    " where " + YOU_SUBJ_ARE + ".";

  the_message_log.add(message);

  return{ true, 1 };
}

Action::StateResult ActionWait::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionWait::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}