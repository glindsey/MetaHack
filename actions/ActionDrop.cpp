#include "stdafx.h"

#include "ActionDrop.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "services/IMessageLog.h"
#include "services/IStrings.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemGeometry.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionDrop ActionDrop::prototype;
  ActionDrop::ActionDrop() : Action("DROP", ActionDrop::create_) {}
  ActionDrop::ActionDrop(EntityId subject) : Action(subject, "DROP") {}
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

  StateResult ActionDrop::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // All checks handled in Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionDrop::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();
    StateResult result = StateResult::Failure();
    std::string message;
    auto subject = getSubject();
    auto object = getObjects().front();
    EntityId location = components.position.existsFor(subject) ? components.position.of(subject).parent() : EntityId::Void;
    arguments["the_location"] = narrator.getDescriptiveString(location);

    /// @todo Handle dropping a certain quantity of an item.

    if (object == subject)
    {
      putMsg(narrator.makeTr("YOU_THROW_SELF_TO_GROUND", arguments));

      /// @todo Possible damage from hurling yourself to the ground!
      arguments["prefix"] = (components.globals.player() == subject) ? tr("PREFIX_FORTUNATELY") : "";
      putMsg(narrator.makeTr("YOU_SEEM_UNHARMED", arguments));
      putMsg(narrator.makeTr("YOU_GET_UP", arguments));
    }
    else if (object != EntityId::Void)
    {
      if (components.inventory.existsFor(location) &&
          components.inventory.of(location).canContain(object))
      {
        if (lua.doSubjectActionObject(subject, *this, object))
        {
          printMessageDo(systems, arguments);

          if (systems.geometry().moveEntityInto(object, location))
          {
            /// @todo Figure out action time.
            result = StateResult::Success();
          }
          else
          {
            putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_UNKNOWN", arguments));

            CLOG(WARNING, "Action") << "Could not drop Entity " << object <<
              " even though beObjectOf returned Success";
          }
        }
        else // Drop failed
        {
          // doSubjectActionObject() will print any relevant messages
        }
      }
      else // can't contain the entity
      {
        // This is mighty strange, but I suppose there might be MapTile spaces in
        // the future that can't contain certain Entities.
        printMessageTry(systems, arguments);

        putMsg(narrator.makeTr("LOCATION_CANT_HOLD_FOO", arguments));
      }
    }

    return result;
  }

  StateResult ActionDrop::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionDrop::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace
