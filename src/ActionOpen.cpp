#include "ActionOpen.h"
#include "Thing.h"
#include "ThingRef.h"

ActionOpen::ActionOpen()
  :
  Action()
{}

ActionOpen::~ActionOpen()
{}

Action::StateResult ActionOpen::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionOpen::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_open(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionOpen::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionOpen::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}