#include "stdafx.h"

#include "ActionMix.h"
#include "Entity.h"
#include "EntityId.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
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

  StateResult ActionMix::do_prebegin_work_(AnyMap& params)
  {
    std::string message;
    auto subject = get_subject();
    auto object1 = get_object();
    auto object2 = get_second_object();

    // Check that they aren't both the same entity.
    if (object1 == object2)
    {
      print_message_try_();

      message = "Those are both the same container!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that neither of them is us.
    if (object1 == subject || object2 == subject)
    {
      print_message_try_();

      message = "But that makes absolutely no sense.";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that both are within reach.
    if (!subject->can_reach(object1) || !subject->can_reach(object2))
    {
      print_message_try_();

      message = make_string("But at least one of them is out of $your(reach).");
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure();
    }

    // Check that both are liquid containers.
    if (!object1->get_intrinsic<bool>("liquid_carrier") || !object2->get_intrinsic<bool>("liquid_carrier"))
    {
      print_message_try_();

      message = "But at least one of them doesn't hold liquid!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure(); //ActionResult::FailureNotLiquidCarrier;
    }

    // Check that neither is empty.
    Inventory& inv1 = object1->get_inventory();
    Inventory& inv2 = object2->get_inventory();
    if (inv1.count() == 0 || inv2.count() == 0)
    {
      print_message_try_();

      message = "But at least one of them is empty!";
      Service<IMessageLog>::get().add(message);

      return StateResult::Failure(); //ActionResult::FailureContainerIsEmpty;
    }

    /// @todo Anything else needed here?
    ///       You need some sort of limbs to mix substances, right?

    return StateResult::Success();
  }

  StateResult ActionMix::do_begin_work_(AnyMap& params)
  {
    /// @todo IMPLEMENT ME
    //message = YOU + CV(" mix ", " mixes ") + LIQUID1 + " with " + LIQUID2 + ".";
    //Service<IMessageLog>::get().add(message);
    //thing1->perform_action_mixed_with_by(thing2, pImpl->ref);

    auto& dict = Service<IStringDictionary>::get();
    Service<IMessageLog>::get().add(dict.get("ACTN_NOT_IMPLEMENTED"));

    return StateResult::Failure();
  }

  StateResult ActionMix::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionMix::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}
