#include "stdafx.h"

#include "ActionTakeOff.h"
#include "components/ComponentManager.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionTakeOff ActionTakeOff::prototype;
  ActionTakeOff::ActionTakeOff() : Action("disrobe", "DISROBE", ActionTakeOff::create_) {}
  ActionTakeOff::ActionTakeOff(EntityId subject) : Action(subject, "disrobe", "DISROBE") {}
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

  StateResult ActionTakeOff::doPreBeginWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::doBeginWorkNVI(GameState& gameState)
  {
    StateResult result = StateResult::Failure();

    auto subject = getSubject();
    auto object = getObject();

    BodyLocation wearLocation = COMPONENTS.bodyparts[subject].getWornLocation(object);
    std::string bodypart_desc = subject->getBodypartDescription(wearLocation);

    // Check if the worn item is bound.
    if (COMPONENTS.magicalBinding.existsFor(object) &&
        COMPONENTS.magicalBinding[object].isAgainst(ComponentMagicalBinding::Against::Disrobing) &&
        COMPONENTS.magicalBinding[object].isActive())
    {
      putMsg(makeTr("YOU_CANT_VERB_FOO_MAGICALLY_BOUND", 
      { subject->getPossessiveString(bodypart_desc) }));

      // Premature exit.
      return result;
    }

    // Try to take off the item.
    /// @todo Disrobing shouldn't be instantaneous.
    if (object->beObjectOf(*this, subject))
    {
      putMsg(makeTr("YOU_CVERB_THE_FOO") + " " + 
             makeTr("YOU_ARE_NOW_WEARING_NOTHING", 
             { subject->getPossessiveString(bodypart_desc) }));
      COMPONENTS.bodyparts[subject].removeWornLocation(wearLocation);
    }

    return result;
  }

  StateResult ActionTakeOff::doFinishWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }

  StateResult ActionTakeOff::doAbortWorkNVI(GameState& gameState)
  {
    return StateResult::Success();
  }
}
