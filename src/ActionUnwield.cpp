#include "stdafx.h"

#include "ActionUnwield.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionUnwield, "unwield", L"unwield")

Action::StateResult ActionUnwield::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();

  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  StringDisplay bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);

  set_object(subject->get_wielding_in(hand));
  auto object = get_object();

  if (object == ThingId::Mu())
  {
    message = make_string(L"$you $are not currently wielding anything!");
    the_message_log.add(message);
    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionUnwield::do_begin_work_(AnyMap& params)
{
  Action::StateResult result = StateResult::Failure();
  StringDisplay message;

  auto subject = get_subject();
  auto object = get_object();

  /// @todo Support wielding in other hand(s).
  unsigned int hand = 0;
  StringDisplay bodypart_desc =
    subject->get_bodypart_description(BodyPart::Hand, hand);

  // Check if the wielded item is bound.
  if (object->get_modified_property<bool>("bound"))
  {
    StringDisplay message;
    message = make_string(L"$you cannot unwield $foo; it is magically bound to $your $0!", { bodypart_desc });
    the_message_log.add(message);

    // Premature exit.
    return result;
  }

  // Try to unwield the item.
  auto lua_result = object->call_lua_function<ActionResult, ThingId>("perform_action_unwielded_by", { subject }, ActionResult::Success);
  if (lua_result == ActionResult::Success)
  {
    StringDisplay message;
    message = make_string(L"$you unwield $foo. $you are now wielding nothing in $your $0.", { bodypart_desc });
    subject->set_wielded(ThingId::Mu(), hand);
    result = Action::StateResult::Success();
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