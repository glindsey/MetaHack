#include "ActionDrop.h"
#include "Thing.h"
#include "ThingRef.h"

ActionDrop::ActionDrop()
  :
  Action()
{}

ActionDrop::~ActionDrop()
{}

Action::StateResult ActionDrop::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionDrop::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_drop(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionDrop::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionDrop::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}