#include "stdafx.h"

#include "ActionShoot.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionShoot ActionShoot::prototype;
  ActionShoot::ActionShoot() : Action("SHOOT", ActionShoot::create_) {}
  ActionShoot::ActionShoot(EntityId subject) : Action(subject, "SHOOT") {}
  ActionShoot::~ActionShoot() {}

  ReasonBool ActionShoot::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isSapient = components.sapience.existsFor(subject);
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }


  std::unordered_set<Trait> const & ActionShoot::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionDirection
    };

    return traits;
  }

  StateResult ActionShoot::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    return StateResult::Failure();
  }

  StateResult ActionShoot::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

