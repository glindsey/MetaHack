#include "ActionWait.h"
#include "Thing.h"
#include "ThingRef.h"

ActionWait::ActionWait(ThingRef subject)
  :
  Action(subject)
{}

ActionWait::~ActionWait()
{}

Action::StateResult ActionWait::do_prebegin_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWait::do_begin_work(AnyMap& params)
{
  bool success;
  unsigned int action_time;

  success = get_subject()->do_move(Direction::Self, action_time);

  return{ success, action_time };
}

Action::StateResult ActionWait::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWait::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}