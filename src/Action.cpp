#include "stdafx.h"

#include "common_types.h"

#include "Action.h"

#include "MessageLog.h"
#include "StringTransforms.h"
#include "Thing.h"
#include "ThingManager.h"
#include "ThingId.h"

std::unordered_map<StringKey, ActionCreator> Action::action_map;

struct Action::Impl
{
  Impl(ThingId subject_)
    :
    state{ Action::State::Pending },
    subject{ subject_ },
    objects{},
    target_thing{ ThingId::Mu() },
    target_direction{ Direction::None },
    quantity{ 1 }
  {}

  /// State of this action.
  Action::State state;

  /// The subject performing the action.
  ThingId subject;

  /// The objects of the action.
  std::vector<ThingId> objects;

  /// Target Thing for the action (if any).
  ThingId target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

Action::Action()
  :
  pImpl{ new Impl(ThingId::Mu()) }
{
  SET_UP_LOGGER("Action", false);
}

Action::Action(ThingId subject)
  :
  pImpl{ new Impl(subject) }
{}

Action::~Action()
{}

ThingId Action::get_subject() const
{
  return pImpl->subject;
}

void Action::set_object(ThingId object)
{
  pImpl->objects.clear();
  pImpl->objects.push_back(object);
}

void Action::set_objects(std::vector<ThingId> objects)
{
  pImpl->objects = objects;
}

std::vector<ThingId> const& Action::get_objects() const
{
  return pImpl->objects;
}

ThingId Action::get_object() const
{
  return pImpl->objects[0];
}

ThingId Action::get_second_object() const
{
  return pImpl->objects[1];
}

bool Action::process(ThingId actor, AnyMap params)
{
  // If entity is currently busy, decrement by one and return.
  int counter_busy = actor->get_base_property<int>("counter_busy");
  if (counter_busy > 0)
  {
    CLOG(TRACE, "Action") << "Thing #" <<
      actor << " (" <<
      actor->get_type() << "): counter_busy = " <<
      counter_busy << "%d, decrementing";

    actor->add_to_base_property<int>("counter_busy", -1);
    return false;
  }

  // Continue running through states until the event is processed, or the
  // target actor is busy.
  while ((pImpl->state != Action::State::Processed) && (counter_busy == 0))
  {
    counter_busy = actor->get_base_property<int>("counter_busy");
    Action::StateResult result{ false, 0 };

    CLOG(TRACE, "Action") << "Thing #" <<
      actor << " (" <<
      actor->get_type().c_str() << "): Action " <<
      get_type().c_str() << " is in state " <<
      str(get_state()) << ", counter_busy = " <<
      counter_busy;

    switch (pImpl->state)
    {
      case Action::State::Pending:
        result = do_prebegin_work(params);

        if (result.success)
        {
          // Update the busy counter.
          pImpl->subject->set_base_property<int>("counter_busy", result.elapsed_time);
          set_state(Action::State::PreBegin);
        }
        else
        {
          // Clear the busy counter.
          pImpl->subject->set_base_property<int>("counter_busy", 0);
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
          pImpl->subject->set_base_property<int>("counter_busy", result.elapsed_time);
          set_state(Action::State::InProgress);
        }
        else
        {
          // Clear the busy counter.
          pImpl->subject->set_base_property<int>("counter_busy", 0);
          set_state(Action::State::PostFinish);
        }
        break;

      case Action::State::InProgress:
        result = do_finish_work(params);

        pImpl->subject->set_base_property<int>("counter_busy", result.elapsed_time);
        set_state(Action::State::PostFinish);
        break;

      case Action::State::Interrupted:
        result = do_abort_work(params);

        pImpl->subject->add_to_base_property<int>("counter_busy", result.elapsed_time);
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

void Action::set_target(ThingId thing) const
{
  pImpl->target_thing = thing;
  pImpl->target_direction = Direction::None;
}

void Action::set_target(Direction direction) const
{
  pImpl->target_thing = ThingId::Mu();
  pImpl->target_direction = direction;
}

void Action::set_quantity(unsigned int quantity) const
{
  pImpl->quantity = quantity;
}

ThingId Action::get_target_thing() const
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
  StringDisplay message;

  auto subject = get_subject();
  auto& objects = get_objects();
  auto location = subject->get_location();
  MapTile* current_tile = subject->get_maptile();
  auto new_direction = get_target_direction();

  if (!subject_can_be_in_limbo())
  {
    // Make sure we're not in limbo!
    if ((location == ThingId::Mu()) || (current_tile == nullptr))
    {
      /// @todo This message could be made less awkward for verbs that take objects.
      message = make_string(L"$you can't $verb because $you $do not exist physically!");
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

          message = make_string(L"However, $obj_pro_foo $foo_is out of $your reach.");
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

          message = make_string(L"However, $obj_pro_foo $foo_is not in $your inventory");
          if (subject->can_reach(object))
          {
            message += L" (pick it up first)";
          }
          message += L".";
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
  the_message_log.add(L"We're sorry, but that action has not yet been implemented.");

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

StringDisplay Action::get_object_string_() const
{
  StringDisplay description;

  if (get_objects().size() == 0)
  {
    description += L"nothing";
  }
  else if (get_objects().size() == 1)
  {
    if (get_object() == get_subject())
    {
      description += get_subject()->get_reflexive_pronoun();
    }
    else
    {
      if (get_object() == ThingId::Mu())
      {
        description += L"nothing";
      }
      else
      {
        if (get_quantity() > 1)
        {
          description += get_quantity() + L" of";
        }
        description += get_object()->get_identifying_string(ArticleChoice::Definite);
      }
    }
  }
  else if (get_objects().size() == 2)
  {
    description += get_object()->get_identifying_string(ArticleChoice::Definite) + L" and " + 
      get_second_object()->get_identifying_string(ArticleChoice::Definite);
  }
  else if (get_objects().size() > 1)
  {
    /// @todo May want to change this depending on whether subject is the player.
    ///       If not, we should print "several items" or something to that effect.
    description += L"the items";
  }
  else
  {
    auto new_direction = get_target_direction();
    if (new_direction != Direction::None)
    {
      description += wstr(new_direction);
    }
  }

  return description;
}

StringDisplay Action::get_target_string_() const
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
    return target->get_identifying_string(ArticleChoice::Definite);
  }
}

void Action::print_message_try_() const
{
  StringDisplay object_string = get_object_string_();
  if (!object_string.empty()) object_string = L" " + object_string;
  StringDisplay message = make_string(L"$you $try to $verb $the_foo.");
  the_message_log.add(message);
}

void Action::print_message_do_() const
{
  StringDisplay object_string = get_object_string_();
  if (!object_string.empty()) object_string = L" " + object_string;
  StringDisplay message = make_string(L"$you $cverb $the_foo.");
  the_message_log.add(message);
}

void Action::print_message_begin_() const
{
  StringDisplay object_string = get_object_string_();
  if (!object_string.empty()) object_string = L" " + object_string;
  StringDisplay message = make_string(L"$you $(cv?begin:begins) to $verb $the_foo.");
  the_message_log.add(message);
}

void Action::print_message_stop_() const
{
  StringDisplay object_string = get_object_string_();
  if (!object_string.empty()) object_string = L" " + object_string;
  StringDisplay message = make_string(L"$you $(cv?stop:stops) $verbing $the_foo.");
  the_message_log.add(message);
}

void Action::print_message_finish_() const
{
  StringDisplay object_string = get_object_string_();
  if (!object_string.empty()) object_string = L" " + object_string;
  StringDisplay message = make_string(L"$you $(cv?finish:finishes) $verbing $the_foo.");
  the_message_log.add(message);
}

void Action::print_message_cant_() const
{
  StringDisplay message = make_string(L"$you can't $verb that!");
  the_message_log.add(message);
}

void Action::register_action_as(StringKey key, ActionCreator creator)
{
  Action::action_map.insert({ key, creator });
}

bool Action::exists(StringKey key)
{
  return Action::action_map.count(key) != 0;
}

std::unique_ptr<Action> Action::create(StringKey key, ThingId subject)
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


StringDisplay Action::make_string(StringDisplay pattern) const
{
  return make_string(pattern, {});
}

/// @todo Implement more tokens; in particular, implement optional string vector.
StringDisplay Action::make_string(StringDisplay pattern, std::vector<StringDisplay> optional_strings) const
{
  StringDisplay new_string = replace_tokens(pattern,
                                            [&](StringDisplay token) -> StringDisplay
  {
    if (token == L"are")
    {
      return get_subject()->choose_verb(L"are", L"is");
    }
    if (token == L"were")
    {
      return get_subject()->choose_verb(L"were", L"was");
    }
    if (token == L"do")
    {
      return get_subject()->choose_verb(L"do", L"does");
    }
    if (token == L"get")
    {
      return get_subject()->choose_verb(L"get", L"gets");
    }
    if (token == L"have")
    {
      return get_subject()->choose_verb(L"have", L"has");
    }
    if (token == L"seem")
    {
      return get_subject()->choose_verb(L"seem", L"seems");
    }
    if (token == L"try")
    {
      return get_subject()->choose_verb(L"try", L"tries");
    }

    if ((token == L"foo_is") || (token == L"foois"))
    {
      return get_object()->choose_verb(L"are", L"is");
    }
    if ((token == L"foo_has") || (token == L"foohas"))
    {
      return get_object()->choose_verb(L"have", L"has");
    }

    if ((token == L"the_foo") || (token == L"thefoo"))
    {
      return get_object_string_();
    }

    if ((token == L"the_foos_location") || (token == L"thefooslocation"))
    {
      return get_object()->get_location()->get_identifying_string(ArticleChoice::Definite);
    }

    if ((token == L"the_target_thing") || (token == L"thetargetthing"))
    {
      return get_target_thing()->get_identifying_string(ArticleChoice::Definite);
    }

    if (token == L"fooself")
    {
      return get_object()->get_self_or_identifying_string(get_subject(), ArticleChoice::Definite);
    }

    if ((token == L"subj_pro_foo") || (token == L"subjprofoo"))
    {
      return get_object()->get_subject_pronoun();
    }

    if ((token == L"obj_pro_foo") || (token == L"objprofoo"))
    {
      return get_object()->get_object_pronoun();
    }

    if (token == L"verb")
    {
      return get_verb();
    }
    if (token == L"verb3")
    {
      return get_verb3();
    }
    if (token == L"verbed")
    {
      return get_verbed();
    }
    if (token == L"verbing")
    {
      return get_verbing();
    }
    if ((token == L"verb_pp") || (token == L"verbpp"))
    {
      return get_verb_pp();
    }
    if (token == L"cverb")
    {
      return (get_subject()->is_third_person() ? get_verb() : get_verb3());
    }
    if (token == L"objcverb")
    {
      return (get_object()->is_third_person() ? get_verb() : get_verb3());
    }

    if (token == L"you")
    {
      return get_subject()->get_you_or_identifying_string();
    }
    if ((token == L"you_subj") || (token == L"yousubj"))
    {
      return get_subject()->get_subject_pronoun();
    }
    if ((token == L"you_obj") || (token == L"youobj"))
    {
      return get_subject()->get_object_pronoun();
    }
    if (token == L"your")
    {
      return get_subject()->get_possessive();
    }
    if (token == L"yourself")
    {
      return get_subject()->get_reflexive_pronoun();
    }

    if (token == L"targdir")
    {
      std::wstringstream ss;
      ss << get_target_direction();
      return ss.str();
    }

    return L"(" + token + L")";
  },
                                            [&](StringDisplay token) -> bool
  {
    if ((token == L"cv") || (token == L"subjcv") || (token == L"subj_cv"))
    {
      return get_subject()->is_third_person();
    }
    if ((token == L"objcv") || (token == L"obj_cv"))
    {
      return get_object()->is_third_person();
    }
    if ((token == L"is_player") || (token == L"isplayer"))
    {
      return get_subject()->is_player();
    }
    if ((token == L"targcv") || (token == L"targ_cv"))
    {
      return get_target_thing()->is_third_person();
    }

    if (token == L"true")
    {
      return true;
    }
    if (token == L"false")
    {
      return false;
    }

    return true;
  });

  return new_string;
}

ActionMap const & Action::get_map()
{
  return Action::action_map;
}