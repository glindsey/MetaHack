#include "stdafx.h"

#include "ActionMix.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"

namespace Actions
{
  ActionMix ActionMix::prototype;
  ActionMix::ActionMix() : Action("mix", "MIX", ActionMix::create_) {}
  ActionMix::ActionMix(EntityId subject) : Action(subject, "mix", "MIX") {}
  ActionMix::~ActionMix() {}

  std::unordered_set<Trait> const & ActionMix::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObjects
    };

    return traits;
  }

  StateResult ActionMix::doPreBeginWorkNVI(AnyMap& params)
  {
    std::string message;
    auto subject = getSubject();
    auto object1 = getObject();
    auto object2 = getSecondObject();

    // Check that they aren't both the same entity.
    if (object1 == object2)
    {
      printMessageTry();

      message = "Those are both the same container!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that neither of them is us.
    if (object1 == subject || object2 == subject)
    {
      printMessageTry();

      message = "But that makes absolutely no sense.";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that both are within reach.
    if (!subject->canReach(object1) || !subject->canReach(object2))
    {
      printMessageTry();

      message = makeString("But at least one of them is out of $your(reach).");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that both are liquid containers.
    if (!object1->getIntrinsic("liquid_carrier").as<bool>() || 
        !object2->getIntrinsic("liquid_carrier").as<bool>())
    {
      printMessageTry();

      message = "But at least one of them doesn't hold liquid!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure(); //ActionResult::FailureNotLiquidCarrier;
    }

    // Check that neither is empty.
    Inventory& inv1 = object1->getInventory();
    Inventory& inv2 = object2->getInventory();
    if (inv1.count() == 0 || inv2.count() == 0)
    {
      printMessageTry();

      message = "But at least one of them is empty!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure(); //ActionResult::FailureContainerIsEmpty;
    }

    /// @todo Anything else needed here?
    ///       You need some sort of limbs to mix substances, right?

    return StateResult::Success();
  }

  StateResult ActionMix::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo IMPLEMENT ME
    //message = YOU + CV(" mix ", " mixes ") + LIQUID1 + " with " + LIQUID2 + ".";
    //Service<IMessageLog>::get().add(message);
    //thing1->perform_action_mixed_with_by(thing2, pImpl->ref);

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return StateResult::Failure();
  }

  StateResult ActionMix::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionMix::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}
