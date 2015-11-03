#ifndef ACTIONIMPL_H
#define ACTIONIMPL_H

#include "ActionType.h"

class ActionImpl
{
public:
  ActionImpl()
    :
    type{ ActionType::None },
    things{ std::vector<ThingRef>{ } },
    target_thing{ ThingRef() },
    target_direction{ Direction::None },
    quantity{ 0 }
  {}

  /// Type of this action.
  ActionType type;

  /// Thing(s) to perform the action on.
  std::vector<ThingRef> things;

  /// Target Thing for the action (if any).
  ThingRef target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};



#endif // ACTIONIMPL_H
