#include "stdafx.h"

#include "ActionLock.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionLock ActionLock::prototype;
  ActionLock::ActionLock() : Action("lock", "LOCK", ActionLock::create_) {}
  ActionLock::ActionLock(EntityId subject) : Action(subject, "lock", "LOCK") {}
  ActionLock::~ActionLock() {}

  std::unordered_set<Trait> const & ActionLock::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionLock::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionLock::doBeginWorkNVI(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return{ success, action_time };
  }

  StateResult ActionLock::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionLock::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}
