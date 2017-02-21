#include "stdafx.h"

#include "ActionShoot.h"
#include "Thing.h"
#include "ThingId.h"
#include "IStringDictionary.h"
#include "Service.h"

ACTION_SRC_BOILERPLATE(ActionShoot, "shoot", "shoot")

Action::StateResult ActionShoot::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionShoot::do_begin_work_(AnyMap& params)
{
  auto& dict = Service<IStringDictionary>::get();
  the_message_log.add(dict.get("NOT_IMPLEMENTED_MSG"));

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