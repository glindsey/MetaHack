#include "stdafx.h"

#include "ActionLock.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionLock ActionLock::prototype;
  ActionLock::ActionLock() : Action("lock", "LOCK", ActionLock::create_) {}
  ActionLock::ActionLock(EntityId subject) : Action(subject, "lock", "LOCK") {}
  ActionLock::~ActionLock() {}

  std::unordered_set<Action::Trait> const & ActionLock::getTraits() const
  {
    static std::unordered_set<Action::Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionLock::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionLock::do_begin_work_(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

    return{ success, action_time };
  }

  StateResult ActionLock::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionLock::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}
