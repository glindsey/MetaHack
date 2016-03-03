#include "stdafx.h"

#include "ActionWield.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionWield, "wield", "wield")

Action::StateResult ActionWield::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  std::string bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);
  ThingRef currently_wielded = subject->get_wielding(hand);

  std::string thing_name = (object != MU) ? get_object_string_() : "nothing";

  // If it is us, or it is what is already being wielded, it means to unwield whatever is wielded.
  if ((object == subject) || (object == currently_wielded))
  {
    set_object(MU);
  }

  // Check that we have hands capable of wielding anything.
  if (subject->get_bodypart_number(BodyPart::Hand) == 0)
  {
    print_message_try_();

    message = YOU_HAVE + " no way to wield anything!";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionWield::do_begin_work_(AnyMap& params)
{
  Action::StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  std::string bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);
  ThingRef currently_wielded = subject->get_wielding(hand);

  bool was_wielding = false;

  // First, check if we're already wielding something.
  if (currently_wielded != MU)
  {
    /// @todo Move all of this into a separate ActionUnwield, and queue it instead.
    // Check if this item is bound.
    if (object->get_property<bool>("bound"))
    {
      std::string message;
      message = YOU + " cannot unwield " + get_object_string_() + "; it is magically bound to " + YOUR + " " + bodypart_desc + "!";
      the_message_log.add(message);

      // Premature exit.
      return result;
    }

    // Try to unwield the old item.
    if (currently_wielded->perform_action_unwielded_by(subject))
    {
      subject->set_wielded(MU, hand);
      was_wielding = true;
    }
    else
    {
      // Premature exit.
      return result;
    }
  }

  // If we HAVE a new item, try to wield it.
  if (object->be_object_of(*this, subject) == ActionResult::Success)
  {
    subject->set_wielded(object, hand);
    message = YOU_ARE + " now wielding " + get_object_string_() + " with " + YOUR + " " + bodypart_desc + ".";
    the_message_log.add(message);

    // If the weapon autobinds, bind it.
    if (object->get_property<bool>("autobinds"))
    {
      object->set_property<bool>("bound", true);
      std::string message;
      message = get_object_string_() + " magically binds itself to " + YOUR + " " + bodypart_desc + "!";
      the_message_log.add(message);
    }

    result = StateResult::Success();
  }

  return result;
}

Action::StateResult ActionWield::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionWield::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}