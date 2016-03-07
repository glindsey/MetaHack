#include "stdafx.h"

#include "Action.h"

#include "MessageLog.h"
#include "Thing.h"
#include "ThingManager.h"
#include "ThingRef.h"

std::unordered_map<std::string, ActionCreator> Action::action_map;

struct Action::Impl
{
  Impl(ThingRef subject_)
    :
    state{ Action::State::Pending },
    subject{ subject_ },
    objects{},
    target_thing{ MU },
    target_direction{ Direction::None },
    quantity{ 1 }
  {}

  /// State of this action.
  Action::State state;

  /// The subject performing the action.
  ThingRef subject;

  /// The objects of the action.
  std::vector<ThingRef> objects;

  /// Target Thing for the action (if any).
  ThingRef target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

Action::Action()
  :
  pImpl{ new Impl(MU) }
{
  SET_UP_LOGGER("Action", true);
}

Action::Action(ThingRef subject)
  :
  pImpl{ new Impl(subject) }
{}

Action::~Action()
{}

ThingRef Action::get_subject() const
{
  return pImpl->subject;
}

void Action::set_object(ThingRef object)
{
  pImpl->objects.clear();
  pImpl->objects.push_back(object);
}

void Action::set_objects(std::vector<ThingRef> objects)
{
  pImpl->objects = objects;
}

std::vector<ThingRef> const& Action::get_objects() const
{
  return pImpl->objects;
}

ThingRef Action::get_object() const
{
  return pImpl->objects[0];
}

ThingRef Action::get_second_object() const
{
  return pImpl->objects[1];
}

bool Action::process(ThingRef actor, AnyMap params)
{
  // If entity is currently busy, decrement by one and return.
  int counter_busy = actor->get_property<int>("counter_busy");
  if (counter_busy > 0)
  {
    CLOG(TRACE, "Action") << "Thing #" <<
      actor.get_id().to_string().c_str() << " (" <<
      actor->get_type().c_str() << "): counter_busy = " <<
      counter_busy << "%d, decrementing";

    actor->add_to_property<int>("counter_busy", -1);
    return false;
  }

  // Perform any type-specific processing.
  // Useful if, for example, your Entity can rise from the dead.
  /// @todo Figure out how to implement this safely.
  //actor->call_lua_function("process", {});

  // Continue running through states until the event is processed, or the
  // target actor is busy.
  while ((pImpl->state != Action::State::Processed) && (counter_busy == 0))
  {
    counter_busy = actor->get_property<int>("counter_busy");
    Action::StateResult result{ false, 0 };

    CLOG(TRACE, "Action") << "Thing #" <<
      actor.get_id().to_string().c_str() << " (" <<
      actor->get_type().c_str() << "): Action " <<
      get_type().c_str() << " is in state " <<
      str(get_state()).c_str() << ", counter_busy = " <<
      counter_busy;

    switch (pImpl->state)
    {
      case Action::State::Pending:
        result = do_prebegin_work(params);

        if (result.success)
        {
          // Update the busy counter.
          pImpl->subject->set_property<int>("counter_busy", result.elapsed_time);
          set_state(Action::State::PreBegin);
        }
        else
        {
          // Clear the busy counter.
          pImpl->subject->set_property<int>("counter_busy", 0);
          set_state(Action::State::PostFinish);
        }
        break;

      case Action::State::PreBegin:
        result = do_begin_work(params);

        // If starting the action succeeded, move to the in-progress state.
        // Otherwise, just go right to post-finish.
        if (result.success)
        {
          // Update the busy counter.
          pImpl->subject->set_property<int>("counter_busy", result.elapsed_time);
          set_state(Action::State::InProgress);
        }
        else
        {
          // Clear the busy counter.
          pImpl->subject->set_property<int>("counter_busy", 0);
          set_state(Action::State::PostFinish);
        }
        break;

      case Action::State::InProgress:
        result = do_finish_work(params);

        pImpl->subject->set_property<int>("counter_busy", result.elapsed_time);
        set_state(Action::State::PostFinish);
        break;

      case Action::State::Interrupted:
        result = do_abort_work(params);

        pImpl->subject->add_to_property<int>("counter_busy", result.elapsed_time);
        set_state(Action::State::PostFinish);
        break;

      case Action::State::PostFinish:
        set_state(Action::State::Processed);
        break;

      default:
        break;
    }
  }

  return true;
}

void Action::set_state(Action::State state)
{
  pImpl->state = state;
}

Action::State Action::get_state()
{
  return pImpl->state;
}

void Action::set_target(ThingRef thing) const
{
  pImpl->target_thing = thing;
  pImpl->target_direction = Direction::None;
}

void Action::set_target(Direction direction) const
{
  pImpl->target_thing = MU;
  pImpl->target_direction = direction;
}

void Action::set_quantity(unsigned int quantity) const
{
  pImpl->quantity = quantity;
}

ThingRef Action::get_target_thing() const
{
  return pImpl->target_thing;
}

Direction Action::get_target_direction() const
{
  return pImpl->target_direction;
}

unsigned int Action::get_quantity() const
{
  return pImpl->quantity;
}

Action::StateResult Action::do_prebegin_work(AnyMap& params)
{
  std::string message;

  auto subject = get_subject();
  auto& objects = get_objects();
  auto location = subject->get_location();
  MapTile* current_tile = subject->get_maptile();
  auto new_direction = get_target_direction();

  if (!subject_can_be_in_limbo())
  {
    // Make sure we're not in limbo!
    if ((location == MU) || (current_tile == nullptr))
    {
      /// @todo This message could be made less awkward for verbs that take objects.
      message = YOU + " can't " + VERB + " because " + YOU_DO + " not exist physically!";
      the_message_log.add(message);
      return StateResult::Failure();
    }
  }

  if (objects.size() > 0)
  {
    for (auto object : objects)
    {
      if (!object_can_be_out_of_reach())
      {
        // Check that each object is within reach.
        if (!subject->can_reach(object))
        {
          print_message_try_();

          message = "However, " + OBJ_PRO_FOO + FOO_IS + " out of " + YOUR + " reach.";
          the_message_log.add(message);

          return StateResult::Failure();
        }
      }

      if (object_must_be_in_inventory())
      {
        // Check that each object is in our inventory.
        if (!subject->get_inventory().contains(object))
        {
          print_message_try_();

          message = "However, " + OBJ_PRO_FOO + FOO_IS + " not in " + YOUR + " inventory";
          if (subject->can_reach(object))
          {
            message += " (pick it up first)";
          }
          message += ".";
          the_message_log.add(message);

          return StateResult::Failure();
        }
      }

      // Check that we can perform this Action on this object.
      if (!object->can_have_action_done_by(subject, *this))
      {
        print_message_try_();
        print_message_cant_();

        return Action::StateResult::Failure();
      }
    }
  }

  auto result = do_prebegin_work_(params);

  return result;
}

Action::StateResult Action::do_begin_work(AnyMap& params)
{
  auto result = do_begin_work_(params);

  return result;
}

Action::StateResult Action::do_finish_work(AnyMap& params)
{
  auto result = do_finish_work_(params);

  return result;
}

Action::StateResult Action::do_abort_work(AnyMap& params)
{
  auto result = do_abort_work_(params);

  return result;
}

Action::StateResult Action::do_prebegin_work_(AnyMap& params)
{
  /// @todo Set counter_busy based on the action being taken and
  ///       the entity's reflexes.
  return Action::StateResult::Success();
}

Action::StateResult Action::do_begin_work_(AnyMap& params)
{
  the_message_log.add("We're sorry, but that action has not yet been implemented.");

  return Action::StateResult::Failure();
}

Action::StateResult Action::do_finish_work_(AnyMap& params)
{
  /// @todo Complete the action here
  return Action::StateResult::Success();
}

Action::StateResult Action::do_abort_work_(AnyMap& params)
{
  /// @todo Handle aborting the action here.
  return Action::StateResult::Success();
}

std::string Action::get_object_string_()
{
  std::string description;

  if (get_objects().size() == 0)
  {
    description += "nothing";
  }
  else if (get_objects().size() == 1)
  {
    if (get_object() == get_subject())
    {
      description += get_subject()->get_reflexive_pronoun();
    }
    else
    {
      if (get_object() == MU)
      {
        description += "nothing";
      }
      else
      {
        if (get_quantity() > 1)
        {
          description += boost::lexical_cast<std::string>(get_quantity()) + " of";
        }
        description += get_object()->get_identifying_string(true);
      }
    }
  }
  else if (get_objects().size() == 2)
  {
    description += get_object()->get_identifying_string(true) + " and " + get_second_object()->get_identifying_string(true);
  }
  else if (get_objects().size() > 1)
  {
    /// @todo May want to change this depending on whether subject is the player.
    ///       If not, we should print "several items" or something to that effect.
    description += "the items";
  }
  else
  {
    auto new_direction = get_target_direction();
    if (new_direction != Direction::None)
    {
      description += str(new_direction);
    }
  }

  return description;
}

std::string Action::get_target_string_()
{
  auto subject = get_subject();
  auto& objects = get_objects();
  auto target = get_target_thing();

  if (target == subject)
  {
    return subject->get_reflexive_pronoun();
  }
  else if ((objects.size() == 1) && (target == get_object()))
  {
    return get_object()->get_reflexive_pronoun();
  }
  else
  {
    return target->get_identifying_string(true);
  }
}

void Action::print_message_try_()
{
  std::string object_string = get_object_string_();
  if (!object_string.empty()) object_string = " " + object_string;
  std::string message = YOU_TRY + " to " + VERB + " " + get_object_string_() + ".";
  the_message_log.add(message);
}

void Action::print_message_do_()
{
  std::string object_string = get_object_string_();
  if (!object_string.empty()) object_string = " " + object_string;
  std::string message = YOU + " " + CV(VERB, VERB3) + " " + get_object_string_() + ".";
  the_message_log.add(message);
}

void Action::print_message_begin_()
{
  std::string object_string = get_object_string_();
  if (!object_string.empty()) object_string = " " + object_string;
  std::string message = YOU + " " + CV("begin", "begins") + " to " + VERB + object_string + ".";
  the_message_log.add(message);
}

void Action::print_message_stop_()
{
  std::string object_string = get_object_string_();
  if (!object_string.empty()) object_string = " " + object_string;
  std::string message = YOU + " " + CV("stop ", "stops ") + VERBING + object_string + ".";
  the_message_log.add(message);
}

void Action::print_message_finish_()
{
  std::string object_string = get_object_string_();
  if (!object_string.empty()) object_string = " " + object_string;
  std::string message = YOU + " " + CV("finish ", "finishes ") + VERBING + object_string + ".";
  the_message_log.add(message);
}

void Action::print_message_cant_()
{
  std::string message = YOU + " can't " + VERB + " that!";
  the_message_log.add(message);
}

void Action::register_action_as(std::string key, ActionCreator creator)
{
  Action::action_map.insert({ key, creator });
}

bool Action::exists(std::string key)
{
  return Action::action_map.count(key) != 0;
}

std::unique_ptr<Action> Action::create(std::string key, ThingRef subject)
{
  if (Action::action_map.count(key) != 0)
  {
    std::unique_ptr<Action> action = (Action::action_map.at(key))(subject);
    return std::move(action);
  }
  else
  {
    throw std::runtime_error("Requested non-existent action " + key);
  }
}

ActionMap const & Action::get_map()
{
  return Action::action_map;
}