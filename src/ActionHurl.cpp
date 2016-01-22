#include "ActionHurl.h"
#include "Thing.h"
#include "ThingRef.h"

ActionHurl::ActionHurl(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionHurl::~ActionHurl()
{}

Action::StateResult ActionHurl::do_prebegin_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionHurl::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionHurl::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionHurl::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}