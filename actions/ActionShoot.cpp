#include "stdafx.h"

#include "ActionShoot.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  //ACTION_SRC_BOILERPLATE(ActionShoot, "shoot", "shoot")
  //ACTION_TRAIT(can_be_subject_verb_object_preposition_direction)

  ActionShoot ActionShoot::prototype;
  ActionShoot::ActionShoot() : Action("shoot", "SHOOT", ActionShoot::create_) {}
  ActionShoot::ActionShoot(EntityId subject) : Action(subject, "shoot", "SHOOT") {}
  ActionShoot::~ActionShoot() {}

  ReasonBool ActionShoot::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

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

  StateResult ActionShoot::doPreBeginWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::doBeginWorkNVI(GameState& gameState)
  {
    putTr("ACTN_NOT_IMPLEMENTED");
    return StateResult::Failure();
  }

  StateResult ActionShoot::doFinishWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionShoot::doAbortWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }
} // end namespace

