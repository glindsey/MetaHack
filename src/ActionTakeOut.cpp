#include "ActionTakeOut.h"
#include "Thing.h"
#include "ThingRef.h"

ActionTakeOut::ActionTakeOut(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionTakeOut::ActionTakeOut(ThingRef subject, std::vector<ThingRef> objects)
  :
  Action(subject, objects)
{}

ActionTakeOut::~ActionTakeOut()
{}

Action::StateResult ActionTakeOut::do_prebegin_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionTakeOut::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  if (thing != ThingManager::get_mu())
  {
    success = get_subject()->do_take_out(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionTakeOut::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionTakeOut::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}