#include "ActionMove.h"
#include "Thing.h"
#include "ThingRef.h"

ActionMove::ActionMove()
  :
  Action()
{}

ActionMove::~ActionMove()
{}

bool ActionMove::target_can_be_thing() const
{
  return false;
}

bool ActionMove::target_can_be_direction() const
{
  return true;
}

Action::StateResult ActionMove::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionMove::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success;
  unsigned int action_time;

  success = actor->do_move(get_target_direction(), action_time);

  return{ success, action_time };
}

Action::StateResult ActionMove::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionMove::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}