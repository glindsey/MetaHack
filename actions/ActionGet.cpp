#include "stdafx.h"

#include "ActionGet.h"
#include "ActionMove.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "services/IMessageLog.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemGeometry.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionGet ActionGet::prototype;
  ActionGet::ActionGet() : Action("GET", ActionGet::create_) {}
  ActionGet::ActionGet(EntityId subject) : Action(subject, "GET") {}
  ActionGet::~ActionGet() {}

  ReasonBool ActionGet::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool canGrasp = components.bodyparts.existsFor(subject) && components.bodyparts.of(subject).hasPrehensileBodyPart();
    std::string reason = canGrasp ? "" : "YOU_HAVE_NO_GRASPING_BODYPARTS"; ///< @todo Add translation key
    return { canGrasp, reason };
  }

  ReasonBool ActionGet::objectIsAllowed(GameState const& gameState) const
  {
    // For now, you can always get an object.
    /// @todo Handle state of matter, movability
    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionGet::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanTakeAQuantity,
      Trait::ObjectMustNotBeInInventory
    };

    return traits;
  }

  StateResult ActionGet::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& narrator = systems.narrator();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    EntityId location = components.position.existsFor(subject) ? components.position.of(subject).parent() : EntityId::Mu();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    /// @todo When picking up, check if our inventory is full-up.
    if (false)
    {
      putMsg(narrator.makeTr("YOU_TRY_TO_VERB_THE_FOO", arguments));
      putMsg(narrator.makeTr("YOUR_INVENTORY_CANT_HOLD_THE_FOO", arguments));
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionGet::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();

    /// @todo Handle getting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    if (lua.doSubjectActionObject(subject, *this, object))
    {
      putMsg(narrator.makeTr("YOU_CVERB_THE_FOO", arguments));
      if (systems.geometry().moveEntityInto(object, subject))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else // could not add to inventory
      {
        putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_UNKNOWN", arguments));

        CLOG(WARNING, "Action") << "Could not move Entity " << object <<
          " even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionGet::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionGet::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace
