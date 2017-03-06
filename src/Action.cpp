#include "stdafx.h"

#include "common_types.h"

#include "Action.h"

#include "IStringDictionary.h"
#include "MessageLog.h"
#include "Service.h"
#include "StringTransforms.h"
#include "Entity.h"
#include "EntityPool.h"
#include "EntityId.h"

std::unordered_map<std::string, ActionCreator> Action::action_map;

struct Action::Impl
{
  Impl(EntityId subject_)
    :
    state{ Action::State::Pending },
    subject{ subject_ },
    objects{},
    target_thing{ EntityId::Mu() },
    target_direction{ Direction::None },
    quantity{ 1 }
  {}

  /// State of this action.
  Action::State state;

  /// The subject performing the action.
  EntityId subject;

  /// The objects of the action.
  std::vector<EntityId> objects;

  /// Target Entity for the action (if any).
  EntityId target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

Action::Action()
  :
  pImpl{ new Impl(EntityId::Mu()) }
{
  SET_UP_LOGGER("Action", false);
}

Action::Action(EntityId subject)
  :
  pImpl{ new Impl(subject) }
{}

Action::~Action()
{}

EntityId Action::get_subject() const
{
  return pImpl->subject;
}

void Action::set_object(EntityId object)
{
  pImpl->objects.clear();
  pImpl->objects.push_back(object);
}

void Action::set_objects(std::vector<EntityId> objects)
{
  pImpl->objects = objects;
}

std::vector<EntityId> const& Action::get_objects() const
{
  return pImpl->objects;
}

EntityId Action::get_object() const
{
  return pImpl->objects[0];
}

EntityId Action::get_second_object() const
{
  return pImpl->objects[1];
}

bool Action::process(EntityId actor, AnyMap params)
{
  // If entity is currently busy, decrement by one and return.
  int counter_busy = actor->get_base_property<int>("counter_busy");
  if (counter_busy > 0)
  {
    CLOG(TRACE, "Action") << "Entity #" <<
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

    CLOG(TRACE, "Action") << "Entity #" <<
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

void Action::set_target(EntityId entity) const
{
  pImpl->target_thing = entity;
  pImpl->target_direction = Direction::None;
}

void Action::set_target(Direction direction) const
{
  pImpl->target_thing = EntityId::Mu();
  pImpl->target_direction = direction;
}

void Action::set_quantity(unsigned int quantity) const
{
  pImpl->quantity = quantity;
}

EntityId Action::get_target_thing() const
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
    if ((location == EntityId::Mu()) || (current_tile == nullptr))
    {
      put_msg(make_string(tr("DONT_EXIST_PHYSICALLY")));
      return StateResult::Failure();
    }
  }

  if (objects.size() > 0)
  {
    for (auto object : objects)
    {
      if (object_can_be_self())
      {
        if (subject == object)
        {
          // If object can be self, we bypass other checks and let the action
          // subclass handle it.
          /// @todo Not sure this is the best option... think more about this later.
          return do_prebegin_work_(params);
        }
      }


      if (!object_can_be_out_of_reach())
      {
        // Check that each object is within reach.
        if (!subject->can_reach(object))
        {
          print_message_try_();

          put_msg(make_string(tr("OBJECT_OUT_OF_REACH")));

          return StateResult::Failure();
        }
      }

      if (object_must_be_in_inventory())
      {
        // Check that each object is in our inventory.
        if (!subject->get_inventory().contains(object))
        {
          print_message_try_();

          message = make_string(tr("OBJECT_NOT_IN_INVENTORY"));
          if (subject->can_reach(object))
          {
            message += tr("PICK_UP_OBJECT_FIRST");
          }
          message += ".";
          put_msg(message);

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
  put_tmsg("ACTION_NOT_IMPLEMENTED");

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

std::string Action::get_object_string_() const
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
      if (get_object() == EntityId::Mu())
      {
        description += "nothing";
      }
      else
      {
        if (get_quantity() > 1)
        {
          description += get_quantity() + " of";
        }
        description += get_object()->get_identifying_string(ArticleChoice::Definite);
      }
    }
  }
  else if (get_objects().size() == 2)
  {
    description += get_object()->get_identifying_string(ArticleChoice::Definite) + " and " +
      get_second_object()->get_identifying_string(ArticleChoice::Definite);
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

std::string Action::get_target_string_() const
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
  put_msg(make_string(tr("ACTION_TRY_TO_VERB_THE_FOO")));
}

void Action::print_message_do_() const
{
  put_msg(make_string(tr("ACTION_CVERB_THE_FOO")));
}

void Action::print_message_begin_() const
{
  put_msg(make_string(tr("ACTION_BEGIN_TO_VERB_THE_FOO")));
}

void Action::print_message_stop_() const
{
  put_msg(make_string(tr("ACTION_STOP_VERBING_THE_FOO")));
}

void Action::print_message_finish_() const
{
  put_msg(make_string(tr("ACTION_FINISH_VERBING_THE_FOO")));
}

void Action::print_message_cant_() const
{
  put_msg(make_string(tr("ACTION_CANT_VERB_THAT")));
}

void Action::register_action_as(std::string key, ActionCreator creator)
{
  Action::action_map.insert({ key, creator });
}

bool Action::exists(std::string key)
{
  return Action::action_map.count(key) != 0;
}

std::unique_ptr<Action> Action::create(std::string key, EntityId subject)
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

std::string Action::make_string(std::string pattern) const
{
  return make_string(pattern, {});
}

std::string Action::make_string(std::string pattern, std::vector<std::string> optional_strings) const
{
  std::string new_string = replace_tokens(pattern,
                                            [&](std::string token) -> std::string
  {
    if (token == "are")
    {
      return get_subject()->choose_verb(tr("VERB_BE_2"), tr("VERB_BE_3"));
    }
    if (token == "were")
    {
      return get_subject()->choose_verb(tr("VERB_BE_P2"), tr("VERB_BE_P3"));
    }
    if (token == "do")
    {
      return get_subject()->choose_verb(tr("VERB_DO_2"), tr("VERB_DO_3"));
    }
    if (token == "get")
    {
      return get_subject()->choose_verb(tr("VERB_GET_2"), tr("VERB_GET_3"));
    }
    if (token == "have")
    {
      return get_subject()->choose_verb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
    }
    if (token == "seem")
    {
      return get_subject()->choose_verb(tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
    }
    if (token == "try")
    {
      return get_subject()->choose_verb(tr("VERB_TRY_2"), tr("VERB_TRY_3"));
    }

    if ((token == "foo_is") || (token == "foois"))
    {
      return get_object()->choose_verb(tr("VERB_BE_2"), tr("VERB_BE_3"));
    }
    if ((token == "foo_has") || (token == "foohas"))
    {
      return get_object()->choose_verb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
    }

    if ((token == "the_foo") || (token == "thefoo"))
    {
      return get_object_string_();
    }

    if ((token == "the_foos_location") || (token == "thefooslocation"))
    {
      return get_object()->get_location()->get_identifying_string(ArticleChoice::Definite);
    }

    if ((token == "the_target_thing") || (token == "thetargetthing"))
    {
      return get_target_string_();
    }

    if (token == "fooself")
    {
      return get_object()->get_self_or_identifying_string(get_subject(), ArticleChoice::Definite);
    }

    if ((token == "subj_pro_foo") || (token == "subjprofoo"))
    {
      return get_object()->get_subject_pronoun();
    }

    if ((token == "obj_pro_foo") || (token == "objprofoo"))
    {
      return get_object()->get_object_pronoun();
    }

    if (token == "verb")
    {
      return get_verb();
    }
    if (token == "verb3")
    {
      return get_verb3();
    }
    if (token == "verbed")
    {
      return get_verbed();
    }
    if (token == "verbing")
    {
      return get_verbing();
    }
    if ((token == "verb_pp") || (token == "verbpp"))
    {
      return get_verb_pp();
    }
    if (token == "cverb")
    {
      return (get_subject()->is_third_person() ? get_verb() : get_verb3());
    }
    if (token == "objcverb")
    {
      return (get_object()->is_third_person() ? get_verb() : get_verb3());
    }

    if (token == "you")
    {
      return get_subject()->get_you_or_identifying_string();
    }
    if ((token == "you_subj") || (token == "yousubj"))
    {
      return get_subject()->get_subject_pronoun();
    }
    if ((token == "you_obj") || (token == "youobj"))
    {
      return get_subject()->get_object_pronoun();
    }
    if (token == "your")
    {
      return get_subject()->get_possessive();
    }
    if (token == "yourself")
    {
      return get_subject()->get_reflexive_pronoun();
    }

    if (token == "targdir")
    {
      std::stringstream ss;
      ss << get_target_direction();
      return ss.str();
    }

    // Check for a numerical token.
    try
    {
      unsigned int converted = static_cast<unsigned int>(std::stoi(token));

      // Check that the optional arguments are at least this size.
      if (converted < optional_strings.size())
      {
        // Return the string passed in.
        return optional_strings.at(converted);
      }
    }
    catch (std::invalid_argument&)
    {
      // Not a number, so bail.
    }
    catch (...)
    {
      // Throw anything else.
      throw;
    }

    // Nothing else matched, return default.
    return "(" + token + ")";
  },
                                            [&](std::string token) -> bool
  {
    if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
    {
      return get_subject()->is_third_person();
    }
    if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
    {
      return get_object()->is_third_person();
    }
    if ((token == "is_player") || (token == "isplayer"))
    {
      return get_subject()->is_player();
    }
    if ((token == "targcv") || (token == "targ_cv"))
    {
      return get_target_thing()->is_third_person();
    }

    if (token == "true")
    {
      return true;
    }
    if (token == "false")
    {
      return false;
    }

    return true;
  });

  return new_string;
}

std::string Action::make_string(EntityId subject, EntityId object, std::string pattern)
{
  return make_string(subject, object, pattern, {});
}

std::string Action::make_string(EntityId subject, EntityId object, std::string pattern, std::vector<std::string> optional_strings)
{
  std::string new_string = replace_tokens(pattern,
                                          [&](std::string token) -> std::string
  {
    if (token == "are")
    {
      return subject->choose_verb(tr("VERB_BE_2"), tr("VERB_BE_3"));
    }
    if (token == "were")
    {
      return subject->choose_verb(tr("VERB_BE_P2"), tr("VERB_BE_P3"));
    }
    if (token == "do")
    {
      return subject->choose_verb(tr("VERB_DO_2"), tr("VERB_DO_3"));
    }
    if (token == "get")
    {
      return subject->choose_verb(tr("VERB_GET_2"), tr("VERB_GET_3"));
    }
    if (token == "have")
    {
      return subject->choose_verb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
    }
    if (token == "seem")
    {
      return subject->choose_verb(tr("VERB_SEEM_2"), tr("VERB_SEEM_3"));
    }
    if (token == "try")
    {
      return subject->choose_verb(tr("VERB_TRY_2"), tr("VERB_TRY_3"));
    }

    if ((token == "foo_is") || (token == "foois"))
    {
      return object->choose_verb(tr("VERB_BE_2"), tr("VERB_BE_3"));
    }
    if ((token == "foo_has") || (token == "foohas"))
    {
      return object->choose_verb(tr("VERB_HAVE_2"), tr("VERB_HAVE_3"));
    }

    if ((token == "the_foo") || (token == "thefoo"))
    {
      return object->get_identifying_string(ArticleChoice::Definite);
    }

    if ((token == "the_foos_location") || (token == "thefooslocation"))
    {
      return object->get_location()->get_identifying_string(ArticleChoice::Definite);
    }

    if (token == "fooself")
    {
      return object->get_self_or_identifying_string(subject, ArticleChoice::Definite);
    }

    if ((token == "subj_pro_foo") || (token == "subjprofoo"))
    {
      return object->get_subject_pronoun();
    }

    if ((token == "obj_pro_foo") || (token == "objprofoo"))
    {
      return object->get_object_pronoun();
    }

    if (token == "you")
    {
      return subject->get_you_or_identifying_string();
    }
    if ((token == "you_subj") || (token == "yousubj"))
    {
      return subject->get_subject_pronoun();
    }
    if ((token == "you_obj") || (token == "youobj"))
    {
      return subject->get_object_pronoun();
    }
    if (token == "your")
    {
      return subject->get_possessive();
    }
    if (token == "yourself")
    {
      return subject->get_reflexive_pronoun();
    }

    // Check for a numerical token.
    try
    {
      unsigned int converted = static_cast<unsigned int>(std::stoi(token));

      // Check that the optional arguments are at least this size.
      if (converted < optional_strings.size())
      {
        // Return the string passed in.
        return optional_strings.at(converted);
      }
    }
    catch (std::invalid_argument&)
    {
      // Not a number, so bail.
    }
    catch (...)
    {
      // Throw anything else.
      throw;
    }

    // Nothing else matched, return default.
    return "(" + token + ")";
  },
                                          [&](std::string token) -> bool
  {
    if ((token == "cv") || (token == "subjcv") || (token == "subj_cv"))
    {
      return subject->is_third_person();
    }
    if ((token == "objcv") || (token == "obj_cv") || (token == "foocv") || (token == "foo_cv"))
    {
      return object->is_third_person();
    }
    if ((token == "is_player") || (token == "isplayer"))
    {
      return subject->is_player();
    }

    if (token == "true")
    {
      return true;
    }
    if (token == "false")
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