#include "ActionShoot.h"
#include "Thing.h"
#include "ThingRef.h"

ActionShoot::ActionShoot(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionShoot::~ActionShoot()
{}

Action::StateResult ActionShoot::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionShoot::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionShoot::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionShoot::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}