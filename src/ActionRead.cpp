#include "ActionRead.h"
#include "Thing.h"
#include "ThingRef.h"

ActionRead::ActionRead()
  :
  Action()
{}

ActionRead::~ActionRead()
{}

Action::StateResult ActionRead::do_prebegin_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionRead::do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult ActionRead::do_finish_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionRead::do_abort_work(ThingRef actor, AnyMap& params)
{
  return{ true, 0 };
}