#include "ActionDrop.h"
#include "Thing.h"
#include "ThingRef.h"

ActionDrop::ActionDrop(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionDrop::ActionDrop(ThingRef subject, ThingRef object, unsigned int quantity)
  :
  Action(subject, object, quantity)
{}

ActionDrop::~ActionDrop()
{}

Action::StateResult ActionDrop::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionDrop::do_begin_work(AnyMap& params)
{
  /// @todo Handle dropping a certain quantity of an item.
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();

  if (thing != ThingManager::get_mu())
  {
    success = get_subject()->do_drop(thing, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionDrop::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionDrop::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}