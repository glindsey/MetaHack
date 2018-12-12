#include "ActionTakeOut.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "services/Service.h"
#include "services/IMessageLog.h"
#include "services/IStrings.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemGeometry.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionTakeOut ActionTakeOut::prototype;
  ActionTakeOut::ActionTakeOut() : Action("REMOVE", ActionTakeOut::create_) {}
  ActionTakeOut::ActionTakeOut(EntityId subject) : Action(subject, "REMOVE") {}
  ActionTakeOut::~ActionTakeOut() {}

  ReasonBool ActionTakeOut::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isSapient = components.sapience.existsFor(subject);
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();

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

  StateResult ActionTakeOut::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& narrator = systems.narrator();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    // Check that the container is not a MapTile or DynamicEntity.
    auto& objectPosition = components.position.of(object);
    if (!objectPosition.isInsideAnotherEntity())
    {
      printMessageTry(systems, arguments);

      if (components.globals.player() == subject)
      {
        message = narrator.makeTr("CONJUNCTION_BUT", arguments) + " ";
      }

      arguments["preposition"] = tr("PREPOSITION_INSIDE");
      arguments["noun"] = tr("NOUN_CONTAINER");
      message += narrator.makeTr("THE_FOO_IS_NOT_PREPOSITION_NOUN", arguments);

      putMsg(message);

      return StateResult::Failure();
    }

    // Check that the container is within reach.
    auto objectContainer = objectPosition.parent();
    if (!systems.geometry()->firstCanReachSecond(subject, objectContainer))
    {
      printMessageTry(systems, arguments);

      putMsg(narrator.makeTr("THE_FOO_IS_OUT_OF_REACH", arguments));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionTakeOut::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();

    /// @todo Handle taking out a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = components.position.of(object).parent();
    auto newLocation = components.position.of(container).parent();

    // Set the target to be the container as a kludge for message printing.
    setTarget(container);

    if (lua.doSubjectActionObject(subject, *this, object))
    {
      if (systems.geometry().moveEntityInto(object, newLocation))
      {
        printMessageDo(systems, arguments);

        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        arguments["preposition"] = tr("PREPOSITION_FROM");
        putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_PREPOSITION_TARGET_UNKNOWN", arguments));
        CLOG(ERROR, "Action") << "Could not move Entity out of Container even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionTakeOut::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOut::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  void ActionTakeOut::printMessageTry(Systems::Manager& systems, json& arguments) const
  {
    auto& narrator = systems.narrator();
    arguments["preposition"] = tr("PREPOSITION_FROM");
    putMsg(narrator.makeTr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", arguments));
  }

  void ActionTakeOut::printMessageDo(Systems::Manager& systems, json& arguments) const
  {
    auto& narrator = systems.narrator();
    arguments["preposition"] = tr("PREPOSITION_FROM");
    putMsg(narrator.makeTr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", arguments));
  }
}
