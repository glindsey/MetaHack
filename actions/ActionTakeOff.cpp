#include "stdafx.h"

#include "ActionTakeOff.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

#include "entity/Entity.h" // needed for beObjectOf()

namespace Actions
{
  ActionTakeOff ActionTakeOff::prototype;
  ActionTakeOff::ActionTakeOff() : Action("DISROBE", ActionTakeOff::create_) {}
  ActionTakeOff::ActionTakeOff(EntityId subject) : Action(subject, "DISROBE") {}
  ActionTakeOff::~ActionTakeOff() {}

  ReasonBool ActionTakeOff::subjectIsCapable(GameState const& gameState) const
  {
    // You can always take off anything.
    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionTakeOff::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbBodypart,
      Trait::ObjectMustBeWorn
    };

    return traits;
  }

  StateResult ActionTakeOff::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& narrator = systems.narrator();
    StateResult result = StateResult::Failure();

    auto subject = getSubject();
    auto object = getObject();

    BodyLocation wearLocation = components.bodyparts.of(subject).getWornLocation(object);
    std::string bodypart_desc = narrator.getBodypartDescription(subject, wearLocation);
    arguments["your_bodypart"] = narrator.getPossessiveString(subject, bodypart_desc);

    // Check if the worn item is bound.
    if (components.magicalBinding.existsFor(object) &&
        components.magicalBinding.of(object).isAgainst(Components::ComponentMagicalBinding::Against::Disrobing) &&
        components.magicalBinding.of(object).isActive())
    {
      putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_MAGICALLY_BOUND", arguments));

      // Premature exit.
      return result;
    }

    // Try to take off the item.
    /// @todo Disrobing shouldn't be instantaneous.
    if (object->beObjectOf(*this, subject))
    {
      components.bodyparts.of(subject).removeWornLocation(wearLocation);
      putMsg(narrator.makeTr("YOU_CVERB_THE_FOO", arguments));
      putMsg(narrator.makeTr("YOU_ARE_NOW_WEARING_NOTHING", arguments));
    }

    return result;
  }

  StateResult ActionTakeOff::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
}
