#include "ActionOpen.h"
#include "Thing.h"
#include "ThingRef.h"

ActionOpen::ActionOpen(ThingRef subject)
  :
  Action(subject)
{}

ActionOpen::ActionOpen(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionOpen::~ActionOpen()
{}

Action::StateResult ActionOpen::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionOpen::do_begin_work(AnyMap& params)
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

Action::StateResult ActionOpen::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionOpen::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}