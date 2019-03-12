#include "ActionUnwield.h"

#include "components/ComponentManager.h"
#include "config/Strings.h"
#include "lua/LuaObject.h"
#include "services/IMessageLog.h"
#include "services/Service.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionUnwield ActionUnwield::prototype;
  ActionUnwield::ActionUnwield() : Action("UNWIELD", ActionUnwield::create_) {}
  ActionUnwield::ActionUnwield(EntityId subject) : Action(subject, "UNWIELD") {}
  ActionUnwield::~ActionUnwield() {}

  ReasonBool ActionUnwield::subjectIsCapable(GameState const& gameState) const
  {
    // You can always unwield anything.
    return { true, "" };
  }

  std::unordered_set<Trait> const & ActionUnwield::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly,
      Trait::CanBeSubjectVerbBodypart,
      Trait::ObjectMustBeWielded
    };

    return traits;
  }

  StateResult ActionUnwield::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // All checks done by Action class via traits.
    return StateResult::Success();
  }

  StateResult ActionUnwield::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    StateResult result = StateResult::Failure();

    auto subject = getSubject();
    auto object = getObject();
    auto& components = gameState.components();
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();

    BodyLocation wieldLocation = components.bodyparts.of(subject).getWieldedLocation(object);
    std::string bodypart_desc = narrator.getBodypartDescription(subject, wieldLocation);
    arguments["your_bodypart"] = narrator.getPossessiveString(subject, bodypart_desc);

    // Check if the wielded item is bound.
    if (components.magicalBinding.existsFor(object) &&
        components.magicalBinding.of(object).isAgainst(Components::ComponentMagicalBinding::Against::Unwielding) &&
        components.magicalBinding.of(object).isActive())
    {
      putMsg(narrator.makeTr("YOU_CANT_VERB_FOO_MAGICALLY_BOUND", arguments));

      // Premature exit.
      return result;
    }

    // Try to unwield the item.
    /// @todo Unwielding shouldn't be instantaneous...?
    if (lua.doSubjectActionObject(subject, *this, object))
    {
      components.bodyparts.of(subject).removeWieldedLocation(wieldLocation);
      putMsg(narrator.makeTr("YOU_ARE_NOW_WIELDING_NOTHING", arguments));
    }

    return result;
  }

  StateResult ActionUnwield::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionUnwield::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

