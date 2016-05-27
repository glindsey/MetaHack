#include "stdafx.h"

#include "ActionShoot.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionShoot, "shoot", L"shoot")

Action::StateResult ActionShoot::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionShoot::do_begin_work_(AnyMap& params)
{
  the_message_log.add(L"We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionShoot::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionShoot::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}