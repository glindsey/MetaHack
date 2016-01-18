#include "ActionAttire.h"
#include "Thing.h"
#include "ThingRef.h"

ActionAttire::ActionAttire()
  :
  Action()
{}

ActionAttire::~ActionAttire()
{}

Action::StateResult ActionAttire::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionAttire::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult ActionAttire::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionAttire::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}