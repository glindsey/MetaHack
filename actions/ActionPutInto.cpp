#include "stdafx.h"

#include "ActionPutInto.h"
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
  ActionPutInto ActionPutInto::prototype;
  ActionPutInto::ActionPutInto() : Action("STORE", ActionPutInto::create_) {}
  ActionPutInto::ActionPutInto(EntityId subject) : Action(subject, "STORE") {}
  ActionPutInto::~ActionPutInto() {}

  ReasonBool ActionPutInto::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isSapient = COMPONENTS.sapience.existsFor(subject);
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();

    if (!isSapient) return { false, "YOU_ARE_NOT_SAPIENT" }; ///< @todo Add translation key
    if (!canGrasp) return { false, "YOU_HAVE_NO_GRASPING_BODYPARTS" }; ///< @todo Add translation key

    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionPutInto::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjectPrepositionTarget,
      Trait::CanBeSubjectVerbObjectsPrepositionTarget,
      Trait::ObjectMustNotBeWielded,
      Trait::ObjectMustNotBeWorn
    };

    return traits;
  }

  StateResult ActionPutInto::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = getTargetThing();
    auto& narrator = systems.narrator();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    // Check that the entity and container aren't the same entity.
    if (object == container)
    {
      if (gameState.components().globals.player() == subject)
      {
        message = narrator.makeTr("YOU_TRY_TO_STORE_THE_FOO_INSIDE_ITSELF_HUMOROUS", arguments);
      }
      else
      {
        message = narrator.makeTr("YOU_TRY_TO_STORE_THE_FOO_INSIDE_ITSELF_INVALID", arguments);
        CLOG(WARNING, "Action") << "NPC tried to store a container in itself!?";
      }

      return StateResult::Failure();
    }

    // Check that the container actually IS a container.
    if (COMPONENTS.inventory.valueOrDefault(container).maxSize() == 0)
    {
      printMessageTry(systems, arguments);
      putMsg(narrator.makeTr("THE_TARGET_IS_NOT_A_CONTAINER", arguments));
      return StateResult::Failure();
    }

    // Check that the entity's location isn't already the container.
    if (COMPONENTS.position[object].parent() == container)
    {
      printMessageTry(systems, arguments);
      putMsg(narrator.makeTr("THE_FOO_IS_ALREADY_IN_THE_TARGET", arguments));
      return StateResult::Failure();
    }

    // Check that the container is within reach.
    if (!systems.geometry()->firstCanReachSecond(subject, container))
    {
      printMessageTry(systems, arguments);
      putMsg(narrator.makeTr("THE_TARGET_IS_OUT_OF_REACH", arguments));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionPutInto::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    /// @todo Handle putting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    auto container = getTargetThing();
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();

    if (lua.doSubjectActionObject(subject, *this, container))
    {
      printMessageDo(systems, arguments);

      if (systems.geometry().moveEntityInto(object, container))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else
      {
        arguments["preposition"] = tr("PREPOSITION_INTO");
        putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_PREPOSITION_TARGET_UNKNOWN", arguments));

        CLOG(ERROR, "Action") << "Could not move Entity into Container even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionPutInto::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionPutInto::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  void ActionPutInto::printMessageTry(Systems::Manager& systems, json& arguments) const
  {
    auto& narrator = systems.narrator();
    arguments["preposition"] = tr("PREPOSITION_INTO");
    putMsg(narrator.makeTr("YOU_TRY_TO_VERB_THE_FOO_PREPOSITION_TARGET", arguments));
  }

  void ActionPutInto::printMessageDo(Systems::Manager& systems, json& arguments) const
  {
    auto& narrator = systems.narrator();
    arguments["preposition"] = tr("PREPOSITION_INTO");
    putMsg(narrator.makeTr("YOU_VERB_THE_FOO_PREPOSITION_TARGET", arguments));
  }
} // end namespace
