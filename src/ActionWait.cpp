#include "ActionWait.h"
#include "Thing.h"
#include "ThingRef.h"

ActionWait::ActionWait()
  :
  Action(Action::Type::Wait)
{}

ActionWait::~ActionWait()
{}

bool ActionWait::target_can_be_thing() const
{
  return false;
}

bool ActionWait::target_can_be_direction() const
{
  return false;
}

Action::StateResult ActionWait::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWait::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  bool success;
  unsigned int action_time;

  success = actor->do_move(Direction::Self, action_time);

  return{ success, action_time };
}

Action::StateResult ActionWait::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionWait::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}