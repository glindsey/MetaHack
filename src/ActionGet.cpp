#include "ActionGet.h"
#include "Thing.h"
#include "ThingRef.h"

ActionGet::ActionGet()
  :
  Action()
{}

ActionGet::~ActionGet()
{}

bool ActionGet::target_can_be_thing() const
{
  return false;
}

bool ActionGet::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionGet::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionGet::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    success = actor->do_pick_up(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionGet::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionGet::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}