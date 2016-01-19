#include "ActionQuaff.h"
#include "Thing.h"
#include "ThingRef.h"

ActionQuaff::ActionQuaff(ThingRef subject)
  :
  Action(subject)
{}

ActionQuaff::ActionQuaff(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionQuaff::~ActionQuaff()
{}

Action::StateResult ActionQuaff::do_prebegin_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionQuaff::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  the_message_log.add("We're sorry, but that action has not yet been implemented.");

#if 0
  if (thing != ThingManager::get_mu())
  {
    success = actor->do_quaff(thing, action_time);
  }
#endif

  return{ success, action_time };
}

Action::StateResult ActionQuaff::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionQuaff::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}