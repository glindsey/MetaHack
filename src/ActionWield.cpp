#include "ActionWield.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionWield, "Wield", "wield")

Action::StateResult ActionWield::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionWield::do_begin_work_(AnyMap& params)
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

Action::StateResult ActionWield::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionWield::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}