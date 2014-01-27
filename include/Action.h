#ifndef ACTION_H
#define ACTION_H

#include <vector>

#include "Direction.h"
#include "ThingId.h"

// Struct describing an action to execute.
struct Action
{
  Action()
  {
    type = Type::None;
    thing_ids.clear();
    target_can_be_direction = false;
    target_can_be_thing = false;
    target = static_cast<ThingId>(0);
    direction = Direction::None;
  }

  enum class Type
  {
    None,
    Wait,
    Attack,
    Drop,
    Eat,
    Move,
    Pickup,
    Quaff,
    Store,
    TakeOut,
    Wield,
    Count
  } type;

  /// Thing(s) to perform the action on.
  std::vector<ThingId> thing_ids;

  /// If true, action can take a Thing as a target.
  bool target_can_be_thing;

  /// If true, action can take a Direction as a target.
  bool target_can_be_direction;

  /// Target Thing for the action (if any).
  ThingId target;

  /// Direction for the action (if any).
  Direction direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

#endif // ACTION_H
