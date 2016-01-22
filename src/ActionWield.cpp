#include "ActionWield.h"
#include "Thing.h"
#include "ThingRef.h"

ActionWield::ActionWield(ThingRef subject)
  :
  Action(subject)
{}

ActionWield::ActionWield(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionWield::~ActionWield()
{}

Action::StateResult ActionWield::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionWield::do_begin_work(AnyMap& params)
{
  bool success = false;
  unsigned int action_time;

  ThingRef thing = get_objects().front();
  if (thing != ThingManager::get_mu())
  {
    /// @todo Implement wielding using other hands.
    success = get_subject()->do_wield(thing, 0, action_time);
  }

  return{ success, action_time };
}

Action::StateResult ActionWield::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionWield::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}