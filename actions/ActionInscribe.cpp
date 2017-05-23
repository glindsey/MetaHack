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

  StateResult ActionInscribe::doPreBeginWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::doBeginWorkNVI(GameState& gameState)
  {
    putTr("ACTN_NOT_IMPLEMENTED");
    return StateResult::Failure();
  }

  StateResult ActionInscribe::doFinishWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::doAbortWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }
} // end namespace
