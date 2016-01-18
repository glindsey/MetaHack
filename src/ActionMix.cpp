#include "ActionMix.h"
#include "Thing.h"
#include "ThingRef.h"

ActionMix::ActionMix()
  :
  Action()
{}

ActionMix::~ActionMix()
{}

Action::StateResult ActionMix::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionMix::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult ActionMix::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionMix::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}