#ifndef ACTION_H

#define ACTION_H

#include "stdafx.h"

#include "App.h"
#include "Direction.h"
#include "GameState.h"
#include "MessageLog.h"
#include "ThingId.h"

// === FORWARD DECLARATIONS ===================================================
class Action;
class Thing;

// === USING DECLARATIONS =====================================================
using ActionCreator = std::function<std::unique_ptr<Action>(ThingId)>;
using ActionMap = std::unordered_map<std::string, ActionCreator>;

// === MESSAGE HELPER MACROS ==================================================
#define YOU       (get_subject()->get_you_or_identifying_string())  // "you" or descriptive noun like "the goblin"
#define YOU_SUBJ  (get_subject()->get_subject_pronoun())     // "you/he/she/it/etc."
#define YOU_OBJ   (get_subject()->get_object_pronoun())      // "you/him/her/it/etc."
#define YOUR      (get_subject()->get_possessive())          // "your/his/her/its/etc."
#define YOURSELF  (get_subject()->get_reflexive_pronoun())   // "yourself/himself/herself/itself/etc."

#define CV(p12, p3)  (get_subject()->choose_verb(p12, p3))   // shortcut for "Subject - Choose Verb"
#define OBJCV(p12, p3)  (get_object()->choose_verb(p12, p3)) // shortcut for "Object - Choose Verb"

#define ARE   (get_subject()->choose_verb(" are", " is"))
#define WERE  (get_subject()->choose_verb(" were", " was"))
#define DO    (get_subject()->choose_verb(" do", " does"))
#define GET   (get_subject()->choose_verb(" get", " gets"))
#define HAVE  (get_subject()->choose_verb(" have", " has"))
#define SEEM  (get_subject()->choose_verb(" seem", " seems"))
#define TRY   (get_subject()->choose_verb(" try", " tries"))

#define FOO_IS    OBJCV(" are", " is")
#define FOO_HAS   OBJCV(" have", " has")

#define IS_PLAYER (get_subject()->is_player())

#define THE_FOO   (get_object_string_())

#define THE_FOOS_LOCATION  (get_object()->get_location()->get_identifying_string(ArticleChoice::Definite))
#define THE_TARGET_THING   (get_target_thing()->get_identifying_string(ArticleChoice::Definite))

#define FOOSELF (get_object()->get_self_or_identifying_string(get_subject(), ArticleChoice::Definite))

#define SUBJ_PRO_FOO  (get_object()->get_subject_pronoun())     // "you/he/she/it/etc."
#define OBJ_PRO_FOO   (get_object()->get_object_pronoun())      // "you/him/her/it/etc."

//#define LIQUID1      (liquid1->get_identifying_string())
//#define LIQUID2      (liquid2->get_identifying_string())

#define YOU_ARE       (YOU + ARE)
#define YOU_WERE      (YOU + WERE)
#define YOU_SUBJ_ARE  (YOU_SUBJ + ARE)
#define YOU_DO        (YOU + DO)
#define YOU_GET       (YOU + GET)
#define YOU_HAVE      (YOU + HAVE)
#define YOU_SEEM      (YOU + SEEM)
#define YOU_TRY       (YOU + TRY)

#define VERB          get_verb()
#define VERB3         get_verb3()
#define VERBING       get_verbing()
#define VERBED        get_verbed()
#define VERBPP        get_verb_pp()

// === BOILERPLATE MACRO(S) ===================================================
/// Boilerplate that goes at the start of each Action source file.
/// @todo Good LORD these are ugly. Can we do this without resorting to macros?
#define ACTION_SRC_BOILERPLATE(T, type, verb)                                 \
  T::T() : Action()                                                           \
  {                                                                           \
    Action::register_action_as(type, &T::create_);                            \
  }                                                                           \
  T::T(ThingId subject) : Action(subject) {}                                  \
  T::~T() {}                                                                  \
  std::string const T::get_type() const                                         \
  {                                                                           \
    return type;                                                              \
  }                                                                           \
  std::string const T::get_verb() const                                     \
  {                                                                           \
    return verb;                                                              \
  }                                                                           \
  T T::prototype;

/// Boilerplate that goes at the start of each Action header file.
/// @todo Good LORD these are ugly. Can we do this without resorting to macros?
#define ACTION_HDR_BOILERPLATE(T)                                             \
  private:                                                                    \
    T();                                                                      \
  public:                                                                     \
    explicit T(ThingId subject);                                             \
    virtual ~T();                                                             \
    virtual std::string const get_type() const override;                        \
    virtual std::string const get_verb() const override;                    \
    static T prototype;

// === ACTION TRAITS ==========================================================

#define CREATE_TRAIT(trait)       virtual bool trait() { return false; }

#define ACTION_TRAIT(trait)       virtual bool trait() override { return true; }

/// Class describing an action to execute.
class Action
  :
  public boost::noncopyable
{
public:

  struct StateResult
  {
    static StateResult Success(unsigned int time = 0)
    {
      return{ true, time };
    }

    static StateResult Failure(unsigned int time = 0)
    {
      return{ false, time };
    }

    bool success;
    unsigned int elapsed_time;
  };

  enum class State
  {
    Pending,      ///< The action is waiting to be executed.
    PreBegin,     ///< Wait state prior to starting the action, to account for reaction time.
    InProgress,   ///< The action has been started and is currently in progress.
    Interrupted,  ///< The action was interrupted while in process and will be aborted.
    PostFinish,   ///< The action is finished and the entity is now in a recovery wait state.
    Processed     ///< The action is totally done and can be popped off the queue.
  };

  explicit Action(ThingId subject);
  virtual ~Action();

  ThingId get_subject() const;

  void set_object(ThingId object);
  void set_objects(std::vector<ThingId> objects);

  std::vector<ThingId> const& get_objects() const;
  ThingId get_object() const;
  ThingId get_second_object() const;

  bool process(ThingId actor, AnyMap params);

  void set_state(Action::State state);
  Action::State get_state();

  void set_target(ThingId thing) const;
  void set_target(Direction direction) const;
  void set_quantity(unsigned int quantity) const;

  ThingId get_target_thing() const;
  Direction get_target_direction() const;
  unsigned int get_quantity() const;

  virtual std::string const get_type() const
  {
    return "[Action]";
  }

  /// Return the first-/second-person singular form of the verb to be performed.
  virtual std::string const get_verb() const
  {
    return "[action]";
  }

  /// Return the third-person singular form of the verb to be performed.
  /// By default, appends an "s" to get_verb().
  virtual std::string const get_verb3() const
  {
    std::string verb = get_verb();
    return verb + "s";
  }

  /// Return the present participle form of the verb to be performed.
  /// By default, tries to use standard compositional rules to make the form.
  virtual std::string const get_verbing() const
  {
    std::string verb = get_verb();
    auto verb_last_char = verb.end();
    --verb_last_char;
    char last_character = *verb_last_char;

    if (std::string("aeiou").find(last_character) != std::string::npos)
    {
      return verb.substr(0, verb.length() - 1) + "ing";
    }
    else
    {
      return verb + "ing";
    }
  }

  /// Return the past form of the verb to be performed.
  /// By default, tries to use standard compositional rules to make the form.
  virtual std::string const get_verbed() const
  {
    std::string verb = get_verb();
    auto verb_last_char = verb.end();
    --verb_last_char;
    char last_character = *verb_last_char;

    if (std::string("aeiou").find(last_character) != std::string::npos)
    {
      return verb.substr(0, verb.length() - 1) + "ed";
    }
    else
    {
      return verb + "ed";
    }
  }

  /// Return the past participle form of the verb to be performed.
  /// By default, returns the past form, as it is the same for most verbs.
  virtual std::string const get_verb_pp() const
  {
    return get_verbed();
  }

  /// Return the adjective form of the verb to be performed.
  /// By default, tries to use standard compositional rules to make the form.
  virtual std::string const get_verbable() const
  {
    std::string verb = get_verb();
    auto verb_last_char = verb.end();
    --verb_last_char;
    char last_character = *verb_last_char;

    if (std::string("aeiou").find(last_character) != std::string::npos)
    {
      return verb.substr(0, verb.length() - 1) + "able";
    }
    else
    {
      return verb + "able";
    }
  }

  CREATE_TRAIT(can_be_subject_only)
    CREATE_TRAIT(can_be_subject_verb_object)
    CREATE_TRAIT(can_be_subject_verb_direction)
    CREATE_TRAIT(can_be_subject_verb_objects)
    CREATE_TRAIT(can_be_subject_verb_object_preposition_target)
    CREATE_TRAIT(can_be_subject_verb_objects_preposition_target)
    CREATE_TRAIT(can_be_subject_verb_object_preposition_bodypart)
    CREATE_TRAIT(can_be_subject_verb_object_preposition_direction)
    CREATE_TRAIT(can_take_a_quantity)
    CREATE_TRAIT(subject_can_be_in_limbo)
    CREATE_TRAIT(object_can_be_out_of_reach)
    CREATE_TRAIT(object_can_be_self)
    CREATE_TRAIT(object_must_be_in_inventory)

    ;

  /// A static method that registers an action subclass in a database.
  /// Required so that Lua scripts can instantiate Actions on Things.
  static void register_action_as(std::string key, ActionCreator creator);

  /// A static method that checks if a key exists.
  static bool exists(std::string key);

  /// A static method that returns an Action associated with a key.
  /// If the requested key does not exist, throws an exception.
  static std::unique_ptr<Action> create(std::string key, ThingId subject);

  /// A method for composing a string from a pattern for an action.
  std::string make_string(std::string pattern, std::vector<std::string> optional_strings) const;
  std::string make_string(std::string pattern) const;

  /// A static method for composing strings from patterns.
  static std::string make_string(ThingId subject, ThingId object, std::string pattern, std::vector<std::string> optional_strings);
  static std::string make_string(ThingId subject, ThingId object, std::string pattern);
 

  /// Get a const reference to the action map.
  static ActionMap const& get_map();

protected:
  /// An Action without a subject; used for prototype registration only.
  Action();

  /// Perform work to be done at the start of the PreBegin state.
  /// This work typically consists of checking whether the action is possible.
  /// If StateResult::success is false, the action was not possible and is cancelled.
  /// If StateResult::success is true, the action is possible, and the target actor
  /// is busy for StateResult::elapsed_time before moving to the InProgress state.
  /// @note do_prebegin_work usually has multiple returns in order to avoid
  ///       the Dreaded Pyramid of Doom. It's just easier that way.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating whether the Action continues.
  StateResult do_prebegin_work(AnyMap& params);

  /// Perform work to be done at the start of the InProgress state.
  /// This is where the action begins.
  /// If StateResult::success is false, the action could not begin and is cancelled.
  /// (However, the PreBegin reaction time has already elapsed, so this is more
  ///  like a critical failure when attempting the action, unlike a failure in
  ///   do_prebegin_work which just means the action couldn't be started at all.)
  /// If StateResult::success is true, the action began, and the target actor is
  /// busy for StateResult::elapsed_time while the action is performed. It then
  /// moves to the PostFinish state.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating whether the Action continues.
  StateResult do_begin_work(AnyMap& params);

  /// Perform work to be done at the end of the InProgress state and the start
  /// of the PostFinish state.
  /// This is where the action ends, and is used for actions that are not
  /// instantaneous in game time, such as eating, drinking, reading, etc.
  /// StateResult::success is ignored.
  /// The target actor is busy for StateResult::elapsed_time as a "refractory"
  /// period.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating the post-Action wait time.
  StateResult do_finish_work(AnyMap& params);

  /// Perform work to be done when an action in the InProgress state is aborted.
  /// This is called when an action is aborted.
  /// StateResult::success is ignored.
  /// The target actor is busy for StateResult::elapsed_time as a "refractory"
  /// period.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating the post-Action wait time.
  StateResult do_abort_work(AnyMap& params);

  /// Overridable portion of do_prebegin_work().
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating whether the Action continues.
  virtual StateResult do_prebegin_work_(AnyMap& params);

  /// Overridable portion of do_begin_work().
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating whether the Action continues.
  virtual StateResult do_begin_work_(AnyMap& params);

  /// Overridable portion of do_finish_work().
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating the post-Action wait time.
  virtual StateResult do_finish_work_(AnyMap& params);

  /// Overridable portion of do_abort_work().
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating the post-Action wait time.
  virtual StateResult do_abort_work_(AnyMap& params);

  /// Describes the object(s) or direction in terms of the subject.
  /// This string will vary based on the presence of objects or a direction
  /// for the action, following this order:
  ///   * If only one object is seen, returns " [OBJECT]."
  ///     - If object = subject, returns " [SUBJECT-REFLEXIVE-PRONOUN]."
  ///     - If the quantity is greater than 1, returns "[QUANTITY] of the [OBJECTS]".
  ///   * If more than one object, returns " the items."
  ///   * If a direction is present, returns " [DIRECTION]."
  ///   * If none of these are present, returns an empty string.
  /// @note The string returned has a leading space on it unless no objects or
  ///       direction are found, in order to make message composition cleaner.
  ///
  /// This method can be overridden if necessary to customize the description for a
  /// particular action.
  virtual std::string get_object_string_() const;

  /// Describes the target in terms of the subject or object.
  /// This string will vary based on the presence of objects or a direction
  /// for the action, following this order:
  ///   * If the target equals the subject, returns "[SUBJECT-REFLEXIVE-PRONOUN]".
  ///   * If the target equals the object, returns "[OBJECT-REFLEXIVE-PRONOUN]".
  ///   * Otherwise returns the target description.
  ///
  /// This method can be overridden if necessary to customize the description for a
  /// particular action.
  virtual std::string get_target_string_() const;

  /// Print a "[SUBJECT] try to [VERB]" message.
  /// The message will vary based on the presence of objects or a direction
  /// for the action, using get_object_string().
  /// "Try to" will be conjugated for the subject as "tries to" if needed.
  ///
  /// This method can be overridden if necessary to customize the message for a
  /// particular action.
  virtual void print_message_try_() const;

  /// Print a "[SUBJECT] [VERB]" message.
  /// The message will vary based on the presence of objects or a direction
  /// for the action, using get_object_string().
  /// VERB will be conjugated for the subject as needed.
  ///
  /// This method can be overridden if necessary to customize the message for a
  /// particular action.
  virtual void print_message_do_() const;

  /// Print a "[SUBJECT] begin to [VERB]" message.
  /// The message will vary based on the presence of objects or a direction
  /// for the action, using get_object_string().
  /// "Begin to" will be conjugated for the subject as "begins to" if needed.
  ///
  /// This method can be overridden if necessary to customize the message for a
  /// particular action.
  virtual void print_message_begin_() const;

  /// Print a "[SUBJECT] stop [VERBING]" message.
  /// The message will vary based on the presence of objects or a direction
  /// for the action, using get_object_string().
  /// "Stop" will be conjugated for the subject as "stops" if needed.
  ///
  /// This method can be overridden if necessary to customize the message for a
  /// particular action.
  virtual void print_message_stop_() const;

  /// Print a "[SUBJECT] finish [VERBING]" message.
  /// The message will vary based on the presence of objects or a direction
  /// for the action, using get_object_string().
  /// "Finish" will be conjugated for the subject as "finishes" if needed.
  ///
  /// This method can be overridden if necessary to customize the message for a
  /// particular action.
  virtual void print_message_finish_() const;

  /// Print a "[SUBJECT] can't [VERB] that!" message.
  /// The message will vary based on the presence of objects or a direction
  /// for the action, using get_object_string().
  /// "Finish" will be conjugated for the subject as "finishes" if needed.
  ///
  /// @todo Finish implementing me, right now the default implementation is too simple.
  /// This method can be overridden if necessary to customize the message for a
  /// particular action.
  virtual void print_message_cant_() const;

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  static ActionMap action_map;
};

inline std::ostream& operator<<(std::ostream& os, Action::State const& s)
{
  switch (s)
  {
    case Action::State::Pending:      os << "Pending"; break;
    case Action::State::PreBegin:     os << "PreBegin"; break;
    case Action::State::InProgress:   os << "InProgress"; break;
    case Action::State::Interrupted:  os << "Interrupted"; break;
    case Action::State::PostFinish:   os << "PostFinish"; break;
    case Action::State::Processed:    os << "Processed"; break;
    default:                          os << "???"; break;
  }

  return os;
}

#endif // ACTION_H
