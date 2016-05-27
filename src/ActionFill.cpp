#include "stdafx.h"

#include "ActionFill.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionFill, "fill", L"fill")

Action::StateResult ActionFill::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionFill::do_begin_work_(AnyMap& params)
{
  the_message_log.add(L"We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionFill::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionFill::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}