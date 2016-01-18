#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>

#include "Direction.h"
#include "ThingRef.h"

// Forward declarations
class Thing;

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

  static inline char const* str(Type const& t)
  {
    switch (t)
    {
      case Type::None:      return "None";
      case Type::Wait:      return "Wait";
      case Type::Attack:    return "Attack";
      case Type::Attire:    return "Attire";
      case Type::Close:     return "Close";
      case Type::Drop:      return "Drop";
      case Type::Eat:       return "Eat";
      case Type::Fill:      return "Fill";
      case Type::Get:       return "Get";
      case Type::Hurl:      return "Hurl";
      case Type::Inscribe:  return "Inscribe";
      case Type::Mix:       return "Mix";
      case Type::Move:      return "Move";
      case Type::Open:      return "Open";
      case Type::PutInto:   return "PutInto";
      case Type::Quaff:     return "Quaff";
      case Type::Read:      return "Read";
      case Type::Shoot:     return "Shoot";
      case Type::TakeOut:   return "TakeOut";
      case Type::Use:       return "Use";
      case Type::Wield:     return "Wield";
      case Type::Count:     return "Count";
      default:              return "???";
    }
  }

  Action(Action::Type type);
  virtual ~Action();

  Action::Type get_type() const;
  void set_things(std::vector<ThingRef> things);
  std::vector<ThingRef> const& get_things() const;
  void add_thing(ThingRef thing);
  bool remove_thing(ThingRef thing);
  void clear_things();

  bool process(ThingRef actor, AnyMap params);

  void set_state(Action::State state);
  Action::State get_state();

  void set_target(ThingRef target);
  void set_target(Direction direction);
  ThingRef const& get_target_thing() const;
  Direction const& get_target_direction() const;
  unsigned int get_quantity() const;
  void set_quantity(unsigned int quantity);

  virtual bool target_can_be_thing() const;
  virtual bool target_can_be_direction() const;

protected:
  bool prebegin_(ThingRef actor, AnyMap& params);
  bool begin_(ThingRef actor, AnyMap& params);
  void finish_(ThingRef actor, AnyMap& params);
  void abort_(ThingRef actor, AnyMap& params);

  virtual StateResult do_prebegin_work(ThingRef actor, AnyMap& params);
  virtual StateResult do_begin_work(ThingRef actor, ThingRef thing, AnyMap& params);
  virtual StateResult do_finish_work(ThingRef actor, AnyMap& params);
  virtual StateResult do_abort_work(ThingRef actor, AnyMap& params);

private:
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // ACTION_H
