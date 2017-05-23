#include "stdafx.h"

#include "ActionTakeOut.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "Service.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionTakeOut ActionTakeOut::prototype;
  ActionTakeOut::ActionTakeOut() : Action("remove", "REMOVE", ActionTakeOut::create_) {}
  ActionTakeOut::ActionTakeOut(EntityId subject) : Action(subject, "remove", "REMOVE") {}
  ActionTakeOut::~ActionTakeOut() {}

  ReasonBool ActionTakeOut::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionTakeOut::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbObjects,
      Trait::ObjectCanBeOutOfReach
    };

    return traits;
  }

  StateResult ActionTakeOut::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    // Check that the container is not a MapTile or DynamicEntity.
    auto& objectPosition = COMPONENTS.position.of(object);
    if (!objectPosition.isInsideAnotherEntity())
    {
      printMessageTry();

      if (gameState.components().globals.player() == subject)
      {
        message = makeTr("CONJUNCTION_BUT") + " ";
      }

      message += makeTr("THE_FOO_IS_NOT_PREPOSITION_NOUN", 
      { 
        makeTr("PREPOSITION_INSIDE"), 
        getIndefArt(makeTr("NOUN_CONTAINER"))
      });

      putMsg(message);

      return StateResult::Failure();
    }

    // Check that the container is within reach.
    auto objectContainer = objectPosition.parent();
    if (!SYSTEMS.spacial()->firstCanReachSecond(subject, objectContainer))
    {
      printMessageTry();

      putTr("THE_FOO_IS_OUT_OF_REACH");
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionTakeOut::doBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    /// @todo Handle taking out a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = COMPONENTS.position[object].parent();
    auto newLocation = COMPONENTS.position[container].parent();

    // Set the target to be the container as a kludge for message printing.
    setTarget(container);

    if (object->beObjectOf(*this, subject))
    {
      if (SYSTEMS.spacial().moveEntityInto(object, newLocation))
      {
        printMessageDo();

        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        putMsg(makeTr("YOU_CANT_VERB_FOO_PREPOSITION_TARGET_UNKNOWN", { "from" }));
        CLOG(ERROR, "Action") << "Could not move Entity out of Container even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionTakeOut::doFinishWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOut::doAbortWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }

  void ActionTakeOut::printMessageTry() const
  {
    putMsg(makeTr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", { "from" }));
  }

  void ActionTakeOut::printMessageDo() const
  {
    putMsg(makeTr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", { "from" }));
  }
}
