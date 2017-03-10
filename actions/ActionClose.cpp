#include "stdafx.h"

#include "ActionClose.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

ACTION_SRC_BOILERPLATE(ActionClose, "close", "close")

Action::StateResult ActionClose::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionClose::do_begin_work_(AnyMap& params)
{
  bool success = false;
  unsigned int action_time = 0;

  auto& dict = Service<IStringDictionary>::get();
  Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

#if 0
  if (entity != EntityId::Mu())
  {
    success = actor->do_close(entity, action_time);
  }
#endif

  return{ success, action_time };
}

Action::StateResult ActionClose::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionClose::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}