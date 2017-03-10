#include "stdafx.h"

#include "ActionShoot.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

ACTION_SRC_BOILERPLATE(ActionShoot, "shoot", "shoot")

Action::StateResult ActionShoot::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionShoot::do_begin_work_(AnyMap& params)
{
  auto& dict = Service<IStringDictionary>::get();
  Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

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