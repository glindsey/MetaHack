#include "stdafx.h"

#include "ActionUnlock.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionUnlock ActionUnlock::prototype;
  ActionUnlock::ActionUnlock() : Action("unlock", "UNLOCK", ActionUnlock::create_) {}
  ActionUnlock::ActionUnlock(EntityId subject) : Action(subject, "unlock", "UNLOCK") {}
  ActionUnlock::~ActionUnlock() {}

  std::unordered_set<Trait> const & ActionUnlock::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionUnlock::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionUnlock::do_begin_work_(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return{ success, action_time };
  }

  StateResult ActionUnlock::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionUnlock::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

