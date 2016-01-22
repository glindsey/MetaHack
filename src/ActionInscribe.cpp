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
  return Action::StateResult::Success();
}

Action::StateResult ActionInscribe::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionInscribe::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionInscribe::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}