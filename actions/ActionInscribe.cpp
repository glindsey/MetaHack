#include "stdafx.h"

#include "ActionInscribe.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionInscribe ActionInscribe::prototype;
  ActionInscribe::ActionInscribe() : Action("inscribe", "WRITE", ActionInscribe::create_) {}
  ActionInscribe::ActionInscribe(EntityId subject) : Action(subject, "inscribe", "WRITE") {}
  ActionInscribe::~ActionInscribe() {}

  std::unordered_set<Trait> const & ActionInscribe::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbDirection,
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionInscribe::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::do_begin_work_(AnyMap& params)
  {
    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTION_NOT_IMPLEMENTED"));

    return StateResult::Failure();
  }

  StateResult ActionInscribe::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
