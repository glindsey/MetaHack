#include "stdafx.h"

#include "ActionFill.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionFill ActionFill::prototype;
  ActionFill::ActionFill() : Action("fill", "FILL", ActionFill::create_) {}
  ActionFill::ActionFill(EntityId subject) : Action(subject, "fill", "FILL") {}
  ActionFill::~ActionFill() {}

  std::unordered_set<Trait> const & ActionFill::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget
    };

    return traits;
  }

  StateResult ActionFill::doPreBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionFill::doBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    putTr("ACTN_NOT_IMPLEMENTED");
    return StateResult::Failure();
  }

  StateResult ActionFill::doFinishWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionFill::doAbortWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

