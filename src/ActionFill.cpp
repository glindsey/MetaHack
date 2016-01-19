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
  return{ true, 0 };
}

Action::StateResult ActionFill::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return{ false, 0 };
}

Action::StateResult ActionFill::do_finish_work(AnyMap& params)
{
  return{ true, 0 };
}

Action::StateResult ActionFill::do_abort_work(AnyMap& params)
{
  return{ true, 0 };
}