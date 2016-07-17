#include "stdafx.h"

#include "ActionPutInto.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionPutInto, "putinto", L"store")

Action::StateResult ActionPutInto::do_prebegin_work_(AnyMap& params)
{
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = get_target_thing();

  // Verify that the Action has an object.
  if (object == ThingId::Mu())
  {
    return StateResult::Failure();
  }

  // Check that the thing and container aren't the same thing.
  if (object == container)
  {
    if (IS_PLAYER)
    {
      message = L"That would be an interesting topological exercise.";
    }
    else
    {
      message = make_string(L"$you $try to store $the_foo inside itself, which seriously shouldn't happen.");
      CLOG(WARNING, "Action") << "NPC tried to store a container in itself!?";
    }

    return StateResult::Failure();
  }

  // Check that the thing is not US!
  if (object == subject)
  {
    if (IS_PLAYER)
    {
      /// @todo Possibly allow player to voluntarily enter a container?
      message = L"I'm afraid you can't do that.  "
        L"(At least, not in this version...)";
    }
    else
    {
      message = make_string(L"$you $try to store $yourself into $the_target_thing, which seriously shouldn't happen.");
      CLOG(WARNING, "Action") << "NPC tried to store self!?";
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is not US!
  if (container == subject)
  {
    if (IS_PLAYER)
    {
      message = L"Store something in yourself?  What do you think you are, a drug mule?";
    }
    else
    {
      message = make_string(L"$you $try to store $the_foo into $yourself, which seriously shouldn't happen.");
      CLOG(WARNING, "Action") << "NPC tried to store into self!?";
    }
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container actually IS a container.
  if (container->get_intrinsic<int>("inventory_size") == 0)
  {
    print_message_try_();

    message = make_string(L"$the_target_thing is not a container!");
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the thing's location isn't already the container.
  if (object->get_location() == container)
  {
    print_message_try_();

    message = make_string(L"$the_foo is already in $the_target_thing!");
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that the container is within reach.
  if (!subject->can_reach(container))
  {
    print_message_try_();

    message = make_string(L"$you cannot reach $the_target_thing.");
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that we're not wielding the item.
  if (subject->is_wielding(object))
  {
    print_message_try_();

    message = make_string(L"$you cannot store something that is currently being wielded.");
    the_message_log.add(message);

    return StateResult::Failure();
  }

  // Check that we're not wearing the item.
  if (subject->has_equipped(object))
  {
    print_message_try_();

    /// @todo Perhaps automatically try to unwield the item before dropping?
    message = make_string(L"$you cannot store something that is currently being worn.");
    the_message_log.add(message);

    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionPutInto::do_begin_work_(AnyMap& params)
{
  /// @todo Handle putting a certain quantity of an item.
  Action::StateResult result = StateResult::Failure();
  StringDisplay message;
  auto subject = get_subject();
  auto object = get_object();
  auto container = get_target_thing();

  if (object->be_object_of(*this, subject, container) == ActionResult::Success)
  {
    print_message_do_();

    if (object->move_into(container))
    {
      /// @todo Figure out action time.
      result = StateResult::Success();
    }
    else
    {
      message = make_string(L"$you could not move $the_foo into $the_target_thing for some inexplicable reason.");
      the_message_log.add(message);

      MAJOR_ERROR("Could not move Thing into Container even though be_object_of returned Success");
    }
  }

  return result;
}

Action::StateResult ActionPutInto::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionPutInto::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

void ActionPutInto::print_message_try_() const
{
  StringDisplay message = make_string(L"$you $try to $verb $the_foo into $the_target_thing.");
  the_message_log.add(message);
}

void ActionPutInto::print_message_do_() const
{
  StringDisplay message = make_string(L"$you $cverb $the_foo into $the_target_thing.");
  the_message_log.add(message);
}