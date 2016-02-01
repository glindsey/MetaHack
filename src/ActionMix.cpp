#include "ActionMix.h"
#include "Thing.h"
#include "ThingRef.h"

ActionMix::ActionMix(ThingRef subject, std::vector<ThingRef> objects)
  :
  Action(subject, objects)
{}

ActionMix::~ActionMix()
{}

Action::StateResult ActionMix::do_prebegin_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object1 = get_object();
  auto object2 = get_second_object();

  // Check that they aren't both the same thing.
  if (object1 == object2)
  {
    return Action::StateResult::Failure(); //ActionResult::FailureCircularReference;
  }

  // Check that neither of them is us.
  if (object1 == subject || object2 == subject)
  {
    return Action::StateResult::Failure(); //ActionResult::FailureSelfReference;
  }

  // Check that both are within reach.
  if (!subject->can_reach(object1) || !subject->can_reach(object2))
  {
    return Action::StateResult::Failure(); //ActionResult::FailureThingOutOfReach;
  }

  // Check that both are liquid containers.
  if (!object1->get_intrinsic<bool>("liquid_carrier") || !object2->get_intrinsic<bool>("liquid_carrier"))
  {
    return Action::StateResult::Failure(); //ActionResult::FailureNotLiquidCarrier;
  }

  // Check that neither is empty.
  Inventory& inv1 = object1->get_inventory();
  Inventory& inv2 = object2->get_inventory();
  if (inv1.count() == 0 || inv2.count() == 0)
  {
    return Action::StateResult::Failure(); //ActionResult::FailureContainerIsEmpty;
  }

  /// @todo Anything else needed here?
  ///       You need some sort of limbs to mix substances, right?

  return Action::StateResult::Success();
}

Action::StateResult ActionMix::do_begin_work(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult ActionMix::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionMix::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}