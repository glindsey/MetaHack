#include "ActionClose.h"
#include "Thing.h"
#include "ThingRef.h"

ActionClose::ActionClose()
  :
  Action()
{}

ActionClose::~ActionClose()
{}

Action::StateResult ActionClose::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionClose::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_close(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionClose::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionClose::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}