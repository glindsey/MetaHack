#include "ActionGet.h"
#include "Thing.h"
#include "ThingRef.h"

ActionGet::ActionGet(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionGet::ActionGet(ThingRef subject, ThingRef object, unsigned int quantity)
  :
  Action(subject, object, quantity)
{}

ActionGet::~ActionGet()
{}

Action::StateResult ActionGet::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionGet::do_begin_work(AnyMap& params)
{
  /// @todo Handle getting a certain quantity of an item.
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  if (thing != ThingManager::get_mu())
  {
    success = get_subject()->do_pick_up(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionGet::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionGet::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}