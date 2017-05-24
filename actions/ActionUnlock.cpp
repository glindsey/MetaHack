#include "stdafx.h"

#include "ActionUnlock.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/SystemManager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionUnlock ActionUnlock::prototype;
  ActionUnlock::ActionUnlock() : Action("UNLOCK", ActionUnlock::create_) {}
  ActionUnlock::ActionUnlock(EntityId subject) : Action(subject, "UNLOCK") {}
  ActionUnlock::~ActionUnlock() {}

  ReasonBool ActionUnlock::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isSapient = components.sapience.existsFor(subject);
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionUnlock::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionUnlock::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionUnlock::doBeginWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    bool success = false;
    unsigned int action_time = 0;

    putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    return{ success, action_time };
  }

  StateResult ActionUnlock::doFinishWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionUnlock::doAbortWorkNVI(GameState& gameState, SystemManager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

