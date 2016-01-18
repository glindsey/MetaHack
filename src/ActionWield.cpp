#include "ActionWield.h"
#include "Thing.h"
#include "ThingRef.h"

ActionWield::ActionWield()
  :
  Action()
{}

ActionWield::~ActionWield()
{}

bool ActionWield::target_can_be_thing() const
{
  return false;
}

bool ActionWield::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionWield::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWield::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  if (thing != ThingManager::get_mu())
  {
    /// @todo Implement wielding using other hands.
    success = actor->do_wield(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionWield::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWield::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}