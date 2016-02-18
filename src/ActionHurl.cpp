#include "ActionHurl.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_BOILERPLATE(ActionHurl)

Action::StateResult ActionHurl::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto new_direction = get_target_direction();

  // Check that it isn't US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      message = "Throw yourself?  Throw yourself what, a party?";
    }
    else
    {
      message = YOU_TRY + " to throw " + YOURSELF +
        ", which seriously shouldn't happen.";
      MINOR_ERROR("NPC tried to throw self!?");
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that it's in our inventory.
  if (!subject->get_inventory().contains(object))
  {
    message = YOU_TRY + " to throw " + THE_FOO + ".";
    the_message_log.add(message);

    message = "But " + THE_FOO + OBJCV(" are", " is") +
      " not actually in " + YOUR +
      " inventory!";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  /// Check that we have limbs capable of throwing.
  if (subject->get_intrinsic<bool>("can_throw"))
  {
    message = YOU_TRY_TO("throw") + THE_FOO + ".";
    the_message_log.add(message);
    message = "But, as " + getIndefArt(subject->get_display_name()) + subject->get_display_name() + "," + YOU_ARE + " not capable of throwing anything.";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  /// Check that we're not wearing the item.
  if (subject->has_equipped(object))
  {
    message = YOU_TRY_TO("throw") + THE_FOO + ".";
    the_message_log.add(message);

    message = YOU + " cannot throw something " + YOU_ARE + "wearing.";
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionHurl::do_begin_work_(AnyMap& params)
{
  auto result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto direction = get_target_direction();
  ThingRef new_location = subject->get_location();

  if (object->perform_action_thrown_by(subject, direction))
  {
    if (object->move_into(new_location))
    {
      message = YOU + CV(" throw ", " throws ") + THE_FOO;
      the_message_log.add(message);

      /// @todo When throwing, set Thing's direction and velocity
      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else
    {
      message = YOU + " could not throw " + THE_FOO + " for some inexplicable reason.";
      the_message_log.add(message);

      MAJOR_ERROR("Could not throw Thing");
    }
  }

  return result;
}

Action::StateResult ActionHurl::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionHurl::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}