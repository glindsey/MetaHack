#include "ActionDrop.h"
#include "Thing.h"
#include "ThingRef.h"

ActionDrop::ActionDrop(ThingRef subject, ThingRef object)
  :
  Action(subject, object)
{}

ActionDrop::ActionDrop(ThingRef subject, ThingRef object, unsigned int quantity)
  :
  Action(subject, object, quantity)
{}

ActionDrop::~ActionDrop()
{}

Action::StateResult ActionDrop::do_prebegin_work(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  ThingRef location = subject->get_location();

  // If it's us, this is a special case. Return success.
  if (subject == object)
  {
    return Action::StateResult::Success();
  }

  // Check that it's in our inventory.
  if (!subject->get_inventory().contains(object))
  {
    message = YOU_TRY_TO("drop") + THE_FOO + ".";
    the_message_log.add(message);

    message = THE_FOO + object->choose_verb(" are ", " is ") +
      "not in " + YOUR + " inventory!";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that we're not wielding the item.
  if (subject->is_wielding(object))
  {
    message = YOU_TRY_TO("drop") + THE_FOO + ".";
    the_message_log.add(message);

    /// @todo Perhaps automatically try to unwield the item before dropping?
    message = YOU + " cannot drop something that is currently being wielded.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that we're not wearing the item.
  if (subject->has_equipped(object))
  {
    message = YOU_TRY_TO("drop") + THE_FOO + ".";
    the_message_log.add(message);

    message = YOU + " cannot drop something that is currently being worn.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  // Check that we can move the item.
  if (!object->is_movable_by(subject))
  {
    message = YOU_TRY_TO("drop") + THE_FOO + ".";
    the_message_log.add(message);

    message = THE_FOO + " cannot be moved.";
    the_message_log.add(message);

    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionDrop::do_begin_work(AnyMap& params)
{
  Action::StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_objects().front();
  ThingRef location = subject->get_location();

  /// @todo Handle dropping a certain quantity of an item.

  if (object == subject)
  {
    message = YOU + CV(" hurl ", " hurls ") + YOURSELF + " to the " +
      location->get_display_name() + "!";
    the_message_log.add(message);
    /// @todo Possible damage from hurling yourself to the ground!
    message = (IS_PLAYER ? "Fortunately, " : "") + YOU_SEEM + " unharmed.";
    the_message_log.add(message);
    message = YOU_GET + " up.";
    the_message_log.add(message);
  }
  else if (object != ThingManager::get_mu())
  {
    if (location->can_contain(object) == ActionResult::Success)
    {
      if (object->perform_action_dropped_by(subject))
      {
        message = YOU + CV(" drop ", " drops ") + THE_FOO + ".";
        the_message_log.add(message);
        if (object->move_into(location))
        {
          /// @todo Figure out action time.
          result = StateResult::Success();
        }
        else
        {
          MAJOR_ERROR("Could not move Thing even though "
                      "is_movable returned Success");
        }
      }
      else // Drop failed
      {
        // perform_action_dropped_by() will print any relevant messages
      }
    }
    else // can't contain the thing
    {
      // This is mighty strange, but I suppose there might be MapTiles in
      // the future that can't contain certain Things.
      message = YOU_TRY_TO("drop") + THE_FOO + ".";
      the_message_log.add(message);

      message = location->get_identifying_string() + " cannot hold " + THE_FOO + ".";
      the_message_log.add(message);
    }
  }

  return result;
}

Action::StateResult ActionDrop::do_finish_work(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionDrop::do_abort_work(AnyMap& params)
{
  return Action::StateResult::Success();
}