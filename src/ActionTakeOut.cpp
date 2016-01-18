#include "ActionTakeOut.h"
#include "Thing.h"
#include "ThingRef.h"

ActionTakeOut::ActionTakeOut()
  :
  Action()
{}

ActionTakeOut::~ActionTakeOut()
{}

bool ActionTakeOut::target_can_be_thing() const
{
  return false;
}

bool ActionTakeOut::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionTakeOut::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionTakeOut::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_take_out(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionTakeOut::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionTakeOut::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}