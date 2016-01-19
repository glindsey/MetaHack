#include "ActionWait.h"
#include "Thing.h"
#include "ThingRef.h"

ActionWait::ActionWait(ThingRef subject)
  :
  Action(subject)
{}

ActionWait::~ActionWait()
{}

Action::StateResult ActionWait::do_prebegin_work(AnyMap& params)
{
  // We can always wait.
  return{ true, 0 };
}

Action::StateResult ActionWait::do_begin_work(AnyMap& params)
{
  std::string message =
    YOU + " successfully" + CV(" stay", " stays") +
    " where " + YOU_SUBJ_ARE + ".";

  the_message_log.add(message);

  return{ true, 1 };
}

Action::StateResult ActionWait::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWait::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}