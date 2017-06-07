#include "stdafx.h"

#include "ActionQuaff.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/SystemJanitor.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

#include "entity/Entity.h" // needed for beObjectOf()

namespace Actions
{
  ActionQuaff ActionQuaff::prototype;
  ActionQuaff::ActionQuaff() : Action("DRINK", ActionQuaff::create_) {}
  ActionQuaff::ActionQuaff(EntityId subject) : Action(subject, "DRINK") {}
  ActionQuaff::~ActionQuaff() {}

  std::unordered_set<Trait> const & ActionQuaff::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection,
      Trait::ObjectCanBeSelf,
      Trait::ObjectMustNotBeEmpty,
      Trait::ObjectMustBeLiquidCarrier
    };

    return traits;
  }

  StateResult ActionQuaff::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // All checks handled by Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionQuaff::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    auto object = getObject();
    auto contents = components.inventory.of(object)[InventorySlot::Zero];

    printMessageBegin(systems, arguments);

    // Do the drinking action here.
    /// @todo We drink from the object, but it's what is inside that is
    ///       actually being consumed. Do we call beObjectOf() on the
    ///       object, or on the object's contents?
    ///       For now I am assuming we do it on the contents, since they
    ///       are what will affect the drinker.
    /// @todo Figure out drinking time. This will vary based on the contents
    ///       being consumed.
    if (contents->beObjectOf(*this, subject))
    {
      systems.janitor().markForDeletion(contents);
      return StateResult::Success();
    }
    else
    {
      printMessageStop(systems, arguments);
      return StateResult::Failure();
    }
  }

  StateResult ActionQuaff::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto object = getObject();

    printMessageFinish(systems, arguments);
    return StateResult::Success();
  }

  StateResult ActionQuaff::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    printMessageStop(systems, arguments);
    return StateResult::Success();
  }
} // end namespace

