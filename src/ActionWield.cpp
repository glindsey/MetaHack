#include "stdafx.h"

#include "ActionWield.h"

#include "ActionUnwield.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionWield, "wield", L"wield")

Action::StateResult ActionWield::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();
  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  StringDisplay bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);
  ThingId currently_wielded = subject->get_wielding_in(hand);

  StringDisplay thing_name = (object != ThingId::Mu()) ? get_object_string_() : L"nothing";

  // If it is us, or it is what is already being wielded, it means to unwield whatever is wielded.
  if ((object == subject) || (object == currently_wielded) || (object == ThingId::Mu()))
  {
    std::unique_ptr<Action> unwieldAction(NEW ActionUnwield(subject));
    subject->queue_action(std::move(unwieldAction));

    return StateResult::Failure();
  }
  else if (currently_wielded != ThingId::Mu())
  {
    StringDisplay message;
    message = make_string(L"$you must unwield what you are currently wielding in your $0 first.", { bodypart_desc });
    the_message_log.add(message);

    return StateResult::Failure();
  }
  
  // Check that we have hands capable of wielding anything.
  if (subject->get_bodypart_number(BodyPart::Hand) == 0)
  {
    print_message_try_();

    message = make_string(L"$you $have no way to wield anything!");
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionWield::do_begin_work_(AnyMap& params)
{
  Action::StateResult result = StateResult::Failure();
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  StringDisplay bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);
  ThingId currently_wielded = subject->get_wielding_in(hand);

  // If we HAVE a new item, try to wield it.
  if (object->be_object_of(*this, subject) == ActionResult::Success)
  {
    subject->set_wielded(object, hand);
    message = YOU_ARE + L" now wielding " + get_object_string_() + L" with " + YOUR + L" " + bodypart_desc + L".";
    the_message_log.add(message);

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