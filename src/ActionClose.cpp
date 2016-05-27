#include "stdafx.h"

#include "ActionClose.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionClose, "close", L"close")

Action::StateResult ActionClose::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionClose::do_begin_work_(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  the_message_log.add(L"We're sorry, but that action has not yet been implemented.");

#if 0
  if (thing != ThingId::Mu())
  {
    success = actor->do_close(thing, action_time);
  }
#endif

  return{ success, action_time };
}

Action::StateResult ActionClose::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionClose::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}