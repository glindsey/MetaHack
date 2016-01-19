#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>

#include "App.h"
#include "Direction.h"
#include "MessageLog.h"
#include "ThingRef.h"

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

  static inline char const* str(State const& s)
  {
    switch (s)
    {
      case State::Pending:      return "Pending";
      case State::PreBegin:     return "PreBegin";
      case State::InProgress:   return "InProgress";
      case State::Interrupted:  return "Interrupted";
      case State::PostFinish:   return "PostFinish";
      case State::Processed:    return "Processed";
      default:                  return "???";
    }
  }

#if 0
  enum class Type
  {
    None,
    Wait,
    Attack,
    Attire,
    Close,
    Drop,
    Eat,
    Fill,
    Get,
    Hurl,
    Inscribe,
    Mix,
    Move,
    Open,
    PutInto,
    Quaff,
    Read,
    Shoot,
    TakeOut,
    Use,
    Wield,
    Count
  };
#endif

  Action(ThingRef subject);
  Action(ThingRef subject, ThingRef object);
  Action(ThingRef subject, ThingRef object, unsigned int quantity);
  Action(ThingRef subject, std::vector<ThingRef> objects);
  virtual ~Action();

  ThingRef get_subject() const;
  std::vector<ThingRef> const& get_objects() const;

  bool process(ThingRef actor, AnyMap params);

  void set_state(Action::State state);
  Action::State get_state();

  void set_target(ThingRef thing) const;
  void set_target(Direction direction) const;
  void set_quantity(unsigned int quantity) const;

  ThingRef const& get_target_thing() const;
  Direction const& get_target_direction() const;
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
  bool prebegin_(AnyMap& params);
  bool begin_(AnyMap& params);
  void finish_(AnyMap& params);
  void abort_(AnyMap& params);

  /// Perform work to be done at the start of the PreBegin state.
  /// This work typically consists of checking whether the action is possible.
  /// If StateResult::success is false, the action was not possible and is cancelled.
  /// If StateResult::success is true, the action is possible, and the target actor
  /// is busy for StateResult::elapsed_time before moving to the InProgress state.
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

#endif // ACTION_H
