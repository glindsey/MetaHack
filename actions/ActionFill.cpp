#include "stdafx.h"

#include "ActionFill.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionFill ActionFill::prototype;
  ActionFill::ActionFill() : Action("FILL", ActionFill::create_) {}
  ActionFill::ActionFill(EntityId subject) : Action(subject, "FILL") {}
  ActionFill::~ActionFill() {}

  std::unordered_set<Trait> const & ActionFill::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget
    };

    return traits;
  }

  StateResult ActionFill::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionFill::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    return StateResult::Failure();
  }

  StateResult ActionFill::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionFill::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

