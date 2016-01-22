#include "ActionAttire.h"
#include "Thing.h"
#include "ThingRef.h"

ActionAttire::ActionAttire(ThingRef subject, ThingRef object)
  :
  Action(subject, { object })
{}

ActionAttire::~ActionAttire()
{}

Action::StateResult ActionAttire::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionAttire::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionAttire::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionAttire::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}