#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>

#include "App.h"
#include "Direction.h"
#include "GameState.h"
#include "MessageLog.h"
#include "ThingRef.h"

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

#define THE_FOO   (get_object()->get_identifying_string(true))
#define A_FOO     (get_object()->get_identifying_string(false))

#define THE_FOOS_LOCATION  (get_object()->get_location()->get_identifying_string(true))
#define THE_TARGET_THING   (get_target_thing()->get_identifying_string(true))

#define FOOSELF (get_object()->get_self_or_identifying_string(get_subject(), true))

#define THE_FOO1  (get_object()->get_identifying_string(true))
#define THE_FOO2  (get_second_object()->get_identifying_string(true))

#define SUBJ_PRO_FOO  (get_subject()->get_subject_pronoun())     // "you/he/she/it/etc."
#define OBJ_PRO_FOO   (get_subject()->get_object_pronoun())      // "you/him/her/it/etc."

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

#define YOU_TRY_TO(verb) (YOU_TRY + " to " + verb + " ")

// Forward declarations
class Thing;

// === ACTION TRAITS ==========================================================

#define CREATE_TRAIT(trait)       virtual bool trait()                    \
                                  {                                       \
                                    return false;                         \
                                  }

#define ACTION_TRAIT(trait)       virtual bool trait() override       \
                                  {                                   \
                                    return true;                      \
                                  }

// Class describing an action to execute.
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

  Action(ThingRef subject);
  Action(ThingRef subject, ThingRef object);
  Action(ThingRef subject, ThingRef object, unsigned int quantity);
  Action(ThingRef subject, std::vector<ThingRef> objects);
  virtual ~Action();

  ThingRef get_subject() const;
  std::vector<ThingRef> const& get_objects() const;
  ThingRef get_object() const;
  ThingRef get_second_object() const;

  bool process(ThingRef actor, AnyMap params);

  void set_state(Action::State state);
  Action::State get_state();

  void set_target(ThingRef thing) const;
  void set_target(Direction direction) const;
  void set_quantity(unsigned int quantity) const;

  ThingRef get_target_thing() const;
  Direction get_target_direction() const;
  unsigned int get_quantity() const;

  virtual std::string get_type() const
  {
    return "???";
  }

  CREATE_TRAIT(can_be_subject_only);
  CREATE_TRAIT(can_be_subject_verb_thing);
  CREATE_TRAIT(can_be_subject_verb_direction);
  CREATE_TRAIT(can_be_subject_verb_things);
  CREATE_TRAIT(can_be_subject_verb_thing_preposition_thing);
  CREATE_TRAIT(can_be_subject_verb_things_preposition_thing);
  CREATE_TRAIT(can_be_subject_verb_thing_preposition_bodypart);
  CREATE_TRAIT(can_be_subject_verb_thing_preposition_direction);
  CREATE_TRAIT(can_take_a_quantity);

protected:

  /// Perform work to be done at the start of the PreBegin state.
  /// This work typically consists of checking whether the action is possible.
  /// If StateResult::success is false, the action was not possible and is cancelled.
  /// If StateResult::success is true, the action is possible, and the target actor
  /// is busy for StateResult::elapsed_time before moving to the InProgress state.
  /// @note do_prebegin_work usually has multiple returns in order to avoid
  ///       the Dreaded Pyramid of Doom. It's just easier that way.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating whether the Action continues.
  virtual StateResult do_prebegin_work(AnyMap& params);

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
  virtual StateResult do_begin_work(AnyMap& params);

  /// Perform work to be done at the end of the InProgress state and the start
  /// of the PostFinish state.
  /// This is where the action ends, and is used for actions that are not
  /// instantaneous in game time, such as eating, drinking, reading, etc.
  /// StateResult::success is ignored.
  /// The target actor is busy for StateResult::elapsed_time as a "refractory"
  /// period.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating the post-Action wait time.
  virtual StateResult do_finish_work(AnyMap& params);

  /// Perform work to be done when an action in the InProgress state is aborted.
  /// This is called when an action is aborted.
  /// StateResult::success is ignored.
  /// The target actor is busy for StateResult::elapsed_time as a "refractory"
  /// period.
  /// @param params Map of parameters for the Action.
  /// @return StateResult indicating the post-Action wait time.
  virtual StateResult do_abort_work(AnyMap& params);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
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
