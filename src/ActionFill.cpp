#include "ActionFill.h"
#include "Thing.h"
#include "ThingRef.h"

ActionFill::ActionFill(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionFill::~ActionFill()
{}

Action::StateResult ActionFill::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionFill::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionFill::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionFill::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}