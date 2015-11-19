#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "Direction.h"
#include "ThingRef.h"

// Forward declarations
class Thing;

// Class describing an action to execute.
class Action
{
public:

  enum class State
  {
    Pending,      ///< The action is waiting to be executed.
    PreBegin,     ///< Wait state prior to starting the action, to account for reaction time.
    InProgress,   ///< The action has been started and is currently in progress.
    PostFinish,   ///< The action is finished and the entity is now in a recovery wait state.
    Processed     ///< The action is totally done and can be popped off the queue.
  };

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

  Action();
  Action(Action::Type type);
  Action(Action const&) = default;
  virtual ~Action() = default;

  void set_type(Action::Type type);
  Action::Type get_type() const;
  void set_things(std::vector<ThingRef> things);
  std::vector<ThingRef> const& get_things() const;
  void add_thing(ThingRef thing);
  bool remove_thing(ThingRef thing);
  void clear_things();

  bool target_can_be_thing() const;
  bool target_can_be_direction() const;
  void set_target(ThingRef target);
  void set_target(Direction direction);
  ThingRef const& get_target_thing() const;
  Direction const& get_target_direction() const;
  unsigned int get_quantity() const;
  void set_quantity(unsigned int quantity);

private:
  /// Type of this action.
  Action::Type m_type;

  /// State of this action.
  Action::State m_state;

  /// Thing(s) to perform the action on.
  std::vector<ThingRef> m_things;

  /// Target Thing for the action (if any).
  ThingRef m_target_thing;

  /// Direction for the action (if any).
  Direction m_target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int m_quantity;
};

#endif // ACTION_H
