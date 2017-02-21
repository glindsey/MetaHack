#include "stdafx.h"

#include "ActionInscribe.h"
#include "Thing.h"
#include "ThingId.h"
#include "IStringDictionary.h"
#include "Service.h"

ACTION_SRC_BOILERPLATE(ActionInscribe, "inscribe", "write")

Action::StateResult ActionInscribe::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionInscribe::do_begin_work_(AnyMap& params)
{
  auto& dict = Service<IStringDictionary>::get();
  the_message_log.add(dict.get("NOT_IMPLEMENTED_MSG"));

  return Action::StateResult::Failure();
}

Action::StateResult ActionInscribe::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionInscribe::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}