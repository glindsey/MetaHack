#include "stdafx.h"

#include "ActionWait.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionWait, "wait", L"wait")

Action::StateResult ActionWait::do_prebegin_work_(AnyMap& params)
{
  // We can always wait.
  return Action::StateResult::Success();
}

Action::StateResult ActionWait::do_begin_work_(AnyMap& params)
{
  StringDisplay message = make_string(L"$you successfully $(cv?stay:stays) where $you_subj $are.");

  //YOU + L" successfully" + CV(L" stay", L" stays") +
  //L" where " + YOU_SUBJ_ARE + L".";

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