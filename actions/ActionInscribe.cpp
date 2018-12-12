#include "ActionInscribe.h"

#include "services/IMessageLog.h"
#include "services/IStrings.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionInscribe ActionInscribe::prototype;
  ActionInscribe::ActionInscribe() : Action("WRITE", ActionInscribe::create_) {}
  ActionInscribe::ActionInscribe(EntityId subject) : Action(subject, "WRITE") {}
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

  StateResult ActionInscribe::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    return StateResult::Failure();
  }

  StateResult ActionInscribe::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionInscribe::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace
