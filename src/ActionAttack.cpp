#include "ActionAttack.h"
#include "Thing.h"
#include "ThingRef.h"

ActionAttack::ActionAttack(ThingRef subject)
  :
  Action(subject)
{}

ActionAttack::ActionAttack(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionAttack::~ActionAttack()
{}

Action::StateResult ActionAttack::do_prebegin_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionAttack::do_begin_work(AnyMap& params)
{
  bool success;
  unsigned int action_time;

  success = get_subject()->do_attack(get_target_direction(), action_time);

  return{ success, action_time };
}

Action::StateResult ActionAttack::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionAttack::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}