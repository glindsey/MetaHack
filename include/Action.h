#ifndef ACTION_H
#define ACTION_H

#include <vector>

#include "Direction.h"
#include "ThingId.h"

struct MoveInfo
{
  Direction direction;
};

// Struct describing an action to execute.
struct Action
{
  enum class Type
  {
    None,
    Wait,
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

  union
  {
    MoveInfo move_info;
  };

  std::vector<ThingId> thing_ids;
};

#endif // ACTION_H
