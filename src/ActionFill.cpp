#include "stdafx.h"

#include "ActionFill.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionFill, "fill", "fill")

Action::StateResult ActionFill::do_prebegin_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionFill::do_begin_work_(AnyMap& params)
{
  auto& dict = Service<IStringDictionary>::get();
  Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

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