#include "ActionUse.h"
#include "Thing.h"
#include "ThingRef.h"

ActionUse::ActionUse(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionUse::~ActionUse()
{}

Action::StateResult ActionUse::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionUse::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  if (thing != ThingManager::get_mu())
  {
    success = get_subject()->do_use(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionUse::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionUse::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}