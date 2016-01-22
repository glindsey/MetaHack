#include "ActionEat.h"
#include "Thing.h"
#include "ThingRef.h"

ActionEat::ActionEat(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionEat::~ActionEat()
{}

Action::StateResult ActionEat::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionEat::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  if (thing != ThingManager::get_mu())
  {
    success = get_subject()->do_eat(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionEat::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionEat::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}