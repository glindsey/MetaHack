#include "ActionInscribe.h"
#include "Thing.h"
#include "ThingRef.h"

ActionInscribe::ActionInscribe(ThingRef subject)
  :
  Action(subject)
{}
ActionInscribe::ActionInscribe(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionInscribe::~ActionInscribe()
{}

Action::StateResult ActionInscribe::do_prebegin_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionInscribe::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult ActionInscribe::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionInscribe::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}