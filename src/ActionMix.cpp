#include "ActionMix.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionMix, "Mix")

Action::StateResult ActionMix::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object1 = get_object();
  auto object2 = get_second_object();

  // Check that we're capable of mixing at all.
  if (subject->get_intrinsic<bool>("can_mix"))
  {
    message = YOU_TRY_TO("mix") + THE_FOO1 + " and " + THE_FOO2 + ".";
    the_message_log.add(message);
    message = "But, as " + getIndefArt(subject->get_display_name()) + subject->get_display_name() + "," + YOU_ARE + " not capable of mixing anything together.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that they aren't both the same thing.
  if (object1 == object2)
  {
    message = YOU_TRY_TO("mix") + "the contents of " + THE_FOO1 + " and " + THE_FOO2 + ".";
    the_message_log.add(message);
    message = "Those are both the same container!";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that neither of them is us.
  if (object1 == subject || object2 == subject)
  {
    message = YOU_TRY_TO("mix") + "the contents of " + THE_FOO1 + " and " + THE_FOO2 + ".";
    the_message_log.add(message);
    message = "But that makes absolutely no sense.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that both are within reach.
  if (!subject->can_reach(object1) || !subject->can_reach(object2))
  {
    message = YOU_TRY_TO("mix") + "the contents of " + THE_FOO1 + " and " + THE_FOO2 + ".";
    the_message_log.add(message);
    message = "But at least one of them is out of " + YOUR + " reach.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that both are liquid containers.
  if (!object1->get_intrinsic<bool>("liquid_carrier") || !object2->get_intrinsic<bool>("liquid_carrier"))
  {
    message = YOU_TRY_TO("mix") + "the contents of " + THE_FOO1 + " and " + THE_FOO2 + ".";
    the_message_log.add(message);
    message = "But at least one of them doesn't hold liquid!";
    the_message_log.add(message);

    return Action::StateResult::Failure(); //ActionResult::FailureNotLiquidCarrier;
  }

  // Check that neither is empty.
  Inventory& inv1 = object1->get_inventory();
  Inventory& inv2 = object2->get_inventory();
  if (inv1.count() == 0 || inv2.count() == 0)
  {
    message = YOU_TRY_TO("mix") + "the contents of " + THE_FOO1 + " and " + THE_FOO2 + ".";
    the_message_log.add(message);
    message = "But at least one of them is empty!";
    the_message_log.add(message);

    return Action::StateResult::Failure(); //ActionResult::FailureContainerIsEmpty;
  }

  /// @todo Anything else needed here?
  ///       You need some sort of limbs to mix substances, right?

  return Action::StateResult::Success();
}

Action::StateResult ActionMix::do_begin_work_(AnyMap& params)
{
  /// @todo IMPLEMENT ME
  //message = YOU + CV(" mix ", " mixes ") + LIQUID1 + " with " + LIQUID2 + ".";
  //the_message_log.add(message);
  //thing1->perform_action_mixed_with_by(thing2, pImpl->ref);

  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionMix::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionMix::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}