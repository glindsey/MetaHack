#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "Direction.h"
#include "ThingManager.h"

// Forward declarations
class Thing;

// Struct describing an action to execute.
struct Action
{
  Action()
  {
    type = Type::None;
    things.clear();
    target_can_be_direction = false;
    target_can_be_thing = false;
    target = TM.get_mu();
    direction = Direction::None;
  }

  enum class Type
  {
    None,
    Wait,
    Attack,
    AttackSafe,   ///< "Safe" attack action, only attacks hostiles
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
  } type;

  /// Thing(s) to perform the action on.
  std::vector< ThingRef > things;

  /// If true, action can take a Thing as a target.
  bool target_can_be_thing;

  /// If true, action can take a Direction as a target.
  bool target_can_be_direction;

  /// Target Thing for the action (if any).
  ThingRef target;

  /// Direction for the action (if any).
  Direction direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

#endif // ACTION_H
