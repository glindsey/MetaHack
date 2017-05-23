#include "stdafx.h"

#include "ActionDrop.h"
#include "ActionMove.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionDrop ActionDrop::prototype;
  ActionDrop::ActionDrop() : Action("drop", "DROP", ActionDrop::create_) {}
  ActionDrop::ActionDrop(EntityId subject) : Action(subject, "drop", "DROP") {}
  ActionDrop::~ActionDrop() {}

  ReasonBool ActionDrop::subjectIsCapable(GameState const& gameState) const
  {
    // An entity can always drop an item.
    return { true, "" };
  }

  ReasonBool ActionDrop::objectIsAllowed(GameState const& gameState) const
  {
    // For now, you can always drop an object.
    /// @todo Handle state of matter, movability
    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionDrop::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanTakeAQuantity,
      Trait::ObjectMustNotBeWielded,
      Trait::ObjectMustNotBeWorn,
      Trait::ObjectMustBeInInventory
    };

    return traits;
  }

  StateResult ActionDrop::doPreBeginWorkNVI(GameState& gameState)
  {
    // All checks handled in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionDrop::doBeginWorkNVI(GameState& gameState)
  {
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObjects().front();
    EntityId location = COMPONENTS.position[subject].parent();

    /// @todo Handle dropping a certain quantity of an item.

    if (object == subject)
    {
      putMsg(makeTr("YOU_THROW_SELF_TO_GROUND", { location->getDisplayName() }));
      /// @todo Possible damage from hurling yourself to the ground!
      putMsg(makeTr("YOU_SEEM_UNHARMED", { (gameState.components().globals.player() == subject) ? tr("PREFIX_FORTUNATELY") : "" }));
      putTr("YOU_GET_UP");
    }
    else if (object != EntityId::Mu())
    {
      if (COMPONENTS.inventory.existsFor(location) &&
          COMPONENTS.inventory[location].canContain(object))
      {
        if (object->beObjectOf(*this, subject))
        {
          printMessageDo();

          if (SYSTEMS.spacial().moveEntityInto(object, location))
          {
            /// @todo Figure out action time.
            result = StateResult::Success();
          }
          else
          {
            putTr("YOU_CANT_VERB_FOO_UNKNOWN");

            CLOG(WARNING, "Action") << "Could not drop Entity " << object <<
              " even though beObjectOf returned Success";
          }
        }
        else // Drop failed
        {
          // beObjectOf() will print any relevant messages
        }
      }
      else // can't contain the entity
      {
        // This is mighty strange, but I suppose there might be MapTiles in
        // the future that can't contain certain Entities.
        printMessageTry();

        putMsg(makeTr("LOCATION_CANT_HOLD_FOO", { location->getDescriptiveString() }));
      }
    }

    return result;
  }

  StateResult ActionDrop::doFinishWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionDrop::doAbortWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }
} // end namespace
