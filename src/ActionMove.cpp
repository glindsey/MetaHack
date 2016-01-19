#include "ActionMove.h"
#include "Thing.h"
#include "ThingRef.h"

ActionMove::ActionMove(ThingRef subject)
  :
  Action(subject)
{}

ActionMove::~ActionMove()
{}

Action::StateResult ActionMove::do_prebegin_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionMove::do_begin_work(AnyMap& params)
{
  bool success;
  unsigned int action_time;

  success = get_subject()->do_move(get_target_direction(), action_time);

  return{ success, action_time };
}

Action::StateResult ActionMove::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionMove::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}