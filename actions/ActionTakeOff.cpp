#include "stdafx.h"

#include "ActionTakeOff.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionTakeOff ActionTakeOff::prototype;
  ActionTakeOff::ActionTakeOff() : Action("takeoff", "UNWEAR", ActionTakeOff::create_) {}
  ActionTakeOff::ActionTakeOff(EntityId subject) : Action(subject, "takeoff", "UNWEAR") {}
  ActionTakeOff::~ActionTakeOff() {}

  std::unordered_set<Trait> const & ActionTakeOff::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbBodypart
    };

    return traits;
  }

  StateResult ActionTakeOff::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::do_begin_work_(AnyMap& params)
  {
    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

    return StateResult::Failure();
  }

  StateResult ActionTakeOff::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}
