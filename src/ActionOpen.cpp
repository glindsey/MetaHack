#include "ActionOpen.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionOpen, "open", "open")

Action::StateResult ActionOpen::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionOpen::do_begin_work_(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  the_message_log.add("We're sorry, but that action has not yet been implemented.");

#if 0
  if (thing != ThingManager::get_mu())
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