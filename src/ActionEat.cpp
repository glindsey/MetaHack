#include "stdafx.h"

#include "ActionEat.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionEat, "eat", L"eat")

Action::StateResult ActionEat::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();

  // Check that it isn't US!
  if (subject == object)
  {
    print_message_try_();

    /// @todo Handle "unusual" cases (e.g. zombies?)
    message = L"But " + YOU + L" really " + CV(L"aren't", L"isn't") + L" that tasty, so " + YOU + CV(L" stop.", L" stops.");
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that we're capable of eating at all.
  if (subject->get_modified_property<bool>("can_eat"))
  {
    print_message_try_();

    message = L"But, as " + getIndefArt(subject->get_display_name()) + subject->get_display_name() + L"," + YOU_ARE + L" not capable of eating.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionEat::do_begin_work_(AnyMap& params)
{
  auto subject = get_subject();
  auto object = get_object();

  print_message_begin_();

  // Do the eating action here.
  /// @todo Figure out eating time.
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

Action::StateResult ActionEat::do_finish_work_(AnyMap& params)
{
  auto object = get_object();

  print_message_finish_();
  object->get_inventory()[INVSLOT_ZERO]->destroy();
  return Action::StateResult::Success();
}

Action::StateResult ActionEat::do_abort_work_(AnyMap& params)
{
  print_message_stop_();
  return Action::StateResult::Success();
}