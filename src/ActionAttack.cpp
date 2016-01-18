#include "ActionAttack.h"
#include "Thing.h"
#include "ThingRef.h"

ActionAttack::ActionAttack()
  :
  Action()
{}

ActionAttack::~ActionAttack()
{}

Action::StateResult ActionAttack::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionAttack::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success;
  unsigned int action_time;

  success = actor->do_attack(get_target_direction(), action_time);

  return{ success, action_time };
}

Action::StateResult ActionAttack::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionAttack::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}