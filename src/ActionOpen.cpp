#include "stdafx.h"

#include "ActionOpen.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionOpen, "open", L"open")

Action::StateResult ActionOpen::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionOpen::do_begin_work_(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  the_message_log.add(L"We're sorry, but that action has not yet been implemented.");

#if 0
  if (thing != ThingId::Mu())
  {
    success = actor->do_open(thing, action_time);
  }
#endif

  return{ success, action_time };
}

Action::StateResult ActionOpen::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionOpen::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}