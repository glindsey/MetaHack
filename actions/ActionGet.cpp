#include "stdafx.h"

#include "ActionGet.h"
#include "ActionMove.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "services/IMessageLog.h"
#include "Service.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionGet ActionGet::prototype;
  ActionGet::ActionGet() : Action("get", "GET", ActionGet::create_) {}
  ActionGet::ActionGet(EntityId subject) : Action(subject, "get", "GET") {}
  ActionGet::~ActionGet() {}

  ReasonBool ActionGet::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool canGrasp = COMPONENTS.bodyparts.existsFor(subject) && COMPONENTS.bodyparts[subject].hasPrehensileBodyPart();
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

  StateResult ActionGet::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    std::string message;
    auto subject = getSubject();
    auto object = getObject();
    EntityId location = COMPONENTS.position[subject].parent();

    // Verify that the Action has an object.
    if (object == EntityId::Mu())
    {
      return StateResult::Failure();
    }

    /// @todo When picking up, check if our inventory is full-up.
    if (false)
    {
      putTr("YOU_TRY_TO_VERB_THE_FOO");
      putTr("YOUR_INVENTORY_CANT_HOLD_THE_FOO");
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  StateResult ActionGet::doBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    /// @todo Handle getting a certain quantity of an item.
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObject();

    if (object->beObjectOf(*this, subject))
    {
      putTr("YOU_CVERB_THE_FOO");
      if (SYSTEMS.spacial().moveEntityInto(object, subject))
      {
        /// @todo Figure out action time.
        result = StateResult::Success();
      }
      else // could not add to inventory
      {
        putTr("YOU_CANT_VERB_FOO_UNKNOWN");

        CLOG(WARNING, "Action") << "Could not move Entity " << object <<
          " even though beObjectOf returned Success";
      }
    }

    return result;
  }

  StateResult ActionGet::doFinishWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }

  StateResult ActionGet::doAbortWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }
} // end namespace
