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

      putTr("THOSE_ARE_BOTH_THE_SAME_CONTAINER");
      return StateResult::Failure();
    }

    // Check that neither of them is us.
    if (object1 == subject || object2 == subject)
    {
      printMessageTry();
      putTr("THAT_MAKES_NO_SENSE");
      return StateResult::Failure();
    }

    // Check that both are within reach.
    if (!subject->canReach(object1) || !subject->canReach(object2))
    {
      printMessageTry();
      putTr("AT_LEAST_ONE_IS_OUT_OF_REACH");
      return StateResult::Failure();
    }

    // Check that both are liquid containers.
    if (!(object1->getIntrinsic("liquid_carrier").get<bool>()) || 
        !(object2->getIntrinsic("liquid_carrier").get<bool>()))
    {
      printMessageTry();

      putTr("AT_LEAST_ONE_IS_NOT_LIQUID_CONTAINER");
      return StateResult::Failure();
    }

    // Check that neither is empty.
    Inventory& inv1 = object1->getInventory();
    Inventory& inv2 = object2->getInventory();
    if (inv1.count() == 0 || inv2.count() == 0)
    {
      printMessageTry();

      putTr("AT_LEAST_ONE_IS_EMPTY");
      return StateResult::Failure();
    }

    /// @todo Anything else needed here?
    ///       You need some sort of limbs to mix substances, right?

    return StateResult::Success();
  }

  StateResult ActionMix::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo IMPLEMENT ME
    //putMsg(makeTr("YOU_MIX_1_WITH_2", { liquid1, liquid2 }));
    //thing1->perform_action_mixed_with_by(thing2, pImpl->ref);

    putTr("ACTN_NOT_IMPLEMENTED");
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
