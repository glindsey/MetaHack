#include "ActionEat.h"
#include "Thing.h"
#include "ThingRef.h"

ActionEat::ActionEat(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionEat::~ActionEat()
{}

Action::StateResult ActionEat::do_prebegin_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();

  // Check that it isn't US!
  if (subject == object)
  {
    message = YOU_TRY_TO("eat") + YOURSELF + ".";
    the_message_log.add(message);

    /// @todo Handle "unusual" cases (e.g. zombies?)
    message = "But " + YOU + " really " + CV("aren't", "isn't") + " that tasty, so " + YOU + CV(" stop.", " stops.");
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that we're capable of eating at all.
  if (subject->get_intrinsic<bool>("can_eat"))
  {
    message = YOU_TRY_TO("eat") + FOO + ".";
    the_message_log.add(message);
    message = "But, as " + getIndefArt(subject->get_display_name()) + subject->get_display_name() + "," + YOU_ARE + " not capable of eating.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that the thing is within reach.
  if (!subject->can_reach(object))
  {
    message = YOU_TRY_TO("eat") + FOO + ", but it is out of " + YOUR + " reach.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that it is edible by us.
  if (!object->is_edible_by(subject))
  {
    message = YOU + " can't eat that!";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionEat::do_begin_work(AnyMap& params)
{
  std::string message;
  bool success = false;
  unsigned int action_time = 0;
  auto subject = get_subject();
  auto object = get_object();

  message = YOU + " begin to eat " + FOO + ".";
  the_message_log.add(message);

  // Do the eating action here.
  /// @todo Figure out eating time.
  ActionResult result = object->perform_action_eaten_by(subject);

  switch (result)
  {
    case ActionResult::Success:
    case ActionResult::SuccessDestroyed:
      return Action::StateResult::Success();

    case ActionResult::Failure:
      message = YOU + " stop eating.";
      the_message_log.add(message);
      return Action::StateResult::Failure();

    default:
      MINOR_ERROR("Unknown ActionResult %d", result);
      return Action::StateResult::Failure();
  }
}

Action::StateResult ActionEat::do_finish_work(AnyMap& params)
{
  std::string message;
  auto object = get_object();

  message = YOU + " finish eating.";
  the_message_log.add(message);
  object->get_inventory().get(INVSLOT_ZERO)->destroy();
  return Action::StateResult::Success();
}

Action::StateResult ActionEat::do_abort_work(AnyMap& params)
{
  std::string message;

  message = YOU + " stop eating.";
  the_message_log.add(message);
  return Action::StateResult::Failure();
}