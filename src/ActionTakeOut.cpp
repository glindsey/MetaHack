#include "stdafx.h"

#include "ActionTakeOut.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

ACTION_SRC_BOILERPLATE(ActionTakeOut, "takeout", "remove")

Action::StateResult ActionTakeOut::do_prebegin_work_(AnyMap& params)
{
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();

  // Verify that the Action has an object.
  if (object == EntityId::Mu())
  {
    return StateResult::Failure();
  }

  // Check that the thing isn't US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      /// @todo Maybe allow player to voluntarily exit a container?
      message = "I'm afraid you can't do that.  "
        "(At least, not in this version...)";
    }
    else
    {
      message = YOU_TRY + " to take " + YOURSELF +
        "out, which seriously shouldn't happen.";
      CLOG(WARNING, "Action") << "NPC tried to take self out!?";
    }
    Service<IMessageLog>::get().add(message);

    return StateResult::Failure();
  }

  // Check that the container is not a MapTile or DynamicEntity.
  if (!object->is_inside_another_thing())
  {
    print_message_try_();

    //message = YOU_TRY + " to remove " + THE_FOO +
    //  " from its container.";
    //Service<IMessageLog>::get().add(message);

    message = "But " + THE_FOO + " is not inside a container!";
    Service<IMessageLog>::get().add(message);

    return StateResult::Failure();
  }

  // Check that the container is within reach.
  if (!subject->can_reach(container))
  {
    print_message_try_();

    message = YOU + " cannot reach " + THE_FOO + ".";
    Service<IMessageLog>::get().add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionTakeOut::do_begin_work_(AnyMap& params)
{
  /// @todo Handle taking out a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  std::string message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = object->get_location();
  auto new_location = container->get_location();

  // Set the target to be the container as a kludge for message printing.
  set_target(container);

  if (object->be_object_of(*this, subject) == ActionResult::Success)
  {
    if (object->move_into(new_location))
    {
      print_message_do_();

      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else
    {
      message = YOU + " could not take " + get_object_string_() + " out of " + get_target_string_() + " for some inexplicable reason.";
      Service<IMessageLog>::get().add(message);

      MAJOR_ERROR("Could not move Entity out of Container even though be_object_of returned Success");
    }
  }

  return result;
}

Action::StateResult ActionTakeOut::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionTakeOut::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

void ActionTakeOut::print_message_try_() const
{
  std::string message = YOU_TRY + " to " + VERB + " " + get_object_string_() + " from " + get_target_string_() + ".";
  Service<IMessageLog>::get().add(message);
}

void ActionTakeOut::print_message_do_() const
{
  std::string message = YOU + " " + CV(VERB, VERB3) + " " + get_object_string_() + " from " + get_target_string_() + ".";
  Service<IMessageLog>::get().add(message);
}