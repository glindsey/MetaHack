#include "stdafx.h"

#include "ActionWait.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

ACTION_SRC_BOILERPLATE(ActionWait, "wait", "wait")

Action::StateResult ActionWait::do_prebegin_work_(AnyMap& params)
{
  // We can always wait.
  return Action::StateResult::Success();
}

Action::StateResult ActionWait::do_begin_work_(AnyMap& params)
{
  std::string message = make_string("$you successfully $(cv?stay:stays) where $you_subj $are.");

  //YOU + " successfully" + CV(" stay", " stays") +
  //" where " + YOU_SUBJ_ARE + ".";

  Service<IMessageLog>::get().add(message);

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