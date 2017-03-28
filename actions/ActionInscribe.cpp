#include "stdafx.h"

#include "ActionInscribe.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

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

  StateResult ActionInscribe::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::doBeginWorkNVI(AnyMap& params)
  {
    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return StateResult::Failure();
  }

  StateResult ActionInscribe::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace
