#include "ActionUse.h"
#include "Thing.h"
#include "ThingRef.h"

ActionUse::ActionUse()
  :
  Action()
{}

ActionUse::~ActionUse()
{}

bool ActionUse::target_can_be_thing() const
{
  return false;
}

bool ActionUse::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionUse::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionUse::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_use(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionUse::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionUse::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}