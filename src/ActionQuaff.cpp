#include "stdafx.h"

#include "ActionQuaff.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionQuaff, "quaff", L"drink from")

Action::StateResult ActionQuaff::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  // Check that it isn't US!
  if (subject == object)
  {
    print_message_try_();

    /// @todo When drinking self, special message if caller is a liquid-based organism.
    message = L"Ewwww... no.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that we're capable of drinking at all.
  if (subject->get_modified_property<bool>("can_drink"))
  {
    print_message_try_();

    message = L"But, as " + getIndefArt(subject->get_display_name()) + subject->get_display_name() + L"," + YOU_ARE + L" not capable of drinking liquids.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that it is something that contains a liquid.
  if (!object->get_intrinsic<bool>("liquid_carrier"))
  {
    print_message_try_();

    message = YOU + L" cannot drink from that!";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that it is not empty.
  Inventory& inv = object->get_inventory();
  if (inv.count() == 0)
  {
    print_message_try_();

    message = L"But " + THE_FOO + FOO_IS + L" empty!";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionQuaff::do_begin_work_(AnyMap& params)
{
  auto subject = get_subject();
  auto object = get_object();

  print_message_begin_();

  // Do the drinking action here.
  /// @todo Figure out drinking time.
  ActionResult result = object->be_object_of(*this, subject);

  switch (result)
  {
    case ActionResult::Success:
    case ActionResult::SuccessDestroyed:
      return Action::StateResult::Success();

    case ActionResult::Failure:
      print_message_stop_();
      return Action::StateResult::Failure();

    default:
      CLOG(WARNING, "Action") << "Unknown ActionResult " << result;
      return Action::StateResult::Failure();
  }
}

Action::StateResult ActionQuaff::do_finish_work_(AnyMap& params)
{
  auto object = get_object();

  print_message_finish_();
  object->get_inventory()[INVSLOT_ZERO]->destroy();
  return Action::StateResult::Success();
}

Action::StateResult ActionQuaff::do_abort_work_(AnyMap& params)
{
  print_message_stop_();
  return Action::StateResult::Success();
}