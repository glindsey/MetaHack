#include "stdafx.h"

#include "ActionLock.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionLock ActionLock::prototype;
  ActionLock::ActionLock() : Action("LOCK", ActionLock::create_) {}
  ActionLock::ActionLock(EntityId subject) : Action(subject, "LOCK") {}
  ActionLock::~ActionLock() {}

  ReasonBool ActionLock::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isSapient = components.sapience.existsFor(subject);
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionLock::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionLock::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionLock::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    bool success = false;
    unsigned int action_time = 0;

    putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    return{ success, action_time };
  }

  StateResult ActionLock::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionLock::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
}
