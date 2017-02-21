#include "stdafx.h"

#include "ActionUnlock.h"
#include "Thing.h"
#include "ThingId.h"
#include "IStringDictionary.h"
#include "Service.h"

ACTION_SRC_BOILERPLATE(ActionUnlock, "unlock", "unlock")

Action::StateResult ActionUnlock::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionUnlock::do_begin_work_(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  auto& dict = Service<IStringDictionary>::get();
  the_message_log.add(dict.get("NOT_IMPLEMENTED_MSG"));

  return{ success, action_time };
}

Action::StateResult ActionUnlock::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionUnlock::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}