#include "ActionEat.h"
#include "Thing.h"
#include "ThingRef.h"

ActionEat::ActionEat()
  :
  Action()
{}

ActionEat::~ActionEat()
{}

Action::StateResult ActionEat::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionEat::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_eat(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionEat::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionEat::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}