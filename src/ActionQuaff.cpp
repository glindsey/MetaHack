#include "ActionQuaff.h"
#include "Thing.h"
#include "ThingRef.h"

ActionQuaff::ActionQuaff()
  :
  Action()
{}

ActionQuaff::~ActionQuaff()
{}

bool ActionQuaff::target_can_be_thing() const
{
  return false;
}

bool ActionQuaff::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionQuaff::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionQuaff::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_quaff(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionQuaff::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionQuaff::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}