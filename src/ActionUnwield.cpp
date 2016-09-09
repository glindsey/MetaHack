#include "stdafx.h"

#include "ActionUnwield.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionUnwield, "unwield", "unwield")

Action::StateResult ActionUnwield::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();

  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  std::string bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);

  set_object(subject->get_wielding_in(hand));
  auto object = get_object();

  if (object == ThingId::Mu())
  {
    message = make_string("$you $are not currently wielding anything!");
    the_message_log.add(message);
    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionUnwield::do_begin_work_(AnyMap& params)
{
  Action::StateResult result = StateResult::Failure();
  std::string message;

  auto subject = get_subject();
  auto object = get_object();

  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  std::string bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);

  // Check if the wielded item is bound.
  if (object->get_modified_property<bool>("bound"))
  {
    std::string message;
    message = make_string("$you cannot unwield $foo; it is magically bound to $your $0!", { bodypart_desc });
    the_message_log.add(message);

    // Premature exit.
    return result;
  }

  // Try to unwield the item.
  auto lua_result = object->be_object_of(*this, subject);
  if (object->be_object_of(*this, subject) == ActionResult::Success)
  {
    std::string message;
    message = make_string("$you unwield $foo. $you are now wielding nothing in $your $0.", { bodypart_desc });
    subject->set_wielded(ThingId::Mu(), hand);
  }

  return result;
}

Action::StateResult ActionUnwield::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionUnwield::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}