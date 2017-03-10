#include "stdafx.h"

#include "ActionAttire.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

ACTION_SRC_BOILERPLATE(ActionAttire, "attire", "wear")

Action::StateResult ActionAttire::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionAttire::do_begin_work_(AnyMap& params)
{
  auto& dict = Service<IStringDictionary>::get();
  Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

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