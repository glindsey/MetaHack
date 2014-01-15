#ifndef ACTION_H
#define ACTION_H

#include <vector>

#include "Direction.h"
#include "ThingId.h"

// Struct describing an action to execute.
struct Action
{
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

  /// Target Thing for the action (if any).
  ThingId target;

  /// Direction for the action (if any).
  Direction direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

#endif // ACTION_H
