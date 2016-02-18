#include "ActionAttire.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_BOILERPLATE(ActionAttire)

Action::StateResult ActionAttire::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionAttire::do_begin_work_(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionAttire::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionAttire::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}