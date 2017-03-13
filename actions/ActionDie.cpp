#include "stdafx.h"

#include "ActionDie.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionDie ActionDie::prototype;
  ActionDie::ActionDie() : Action("die", "DIE", ActionDie::create_) {}
  ActionDie::ActionDie(EntityId subject) : Action(subject, "die", "DIE") {}
  ActionDie::~ActionDie() {}

  std::unordered_set<Trait> const & ActionDie::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly,
      Trait::SubjectCanBeInLimbo
    };

    return traits;
  }

  StateResult ActionDie::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::do_begin_work_(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return{ success, action_time };
  }

  StateResult ActionDie::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}

