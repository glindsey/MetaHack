#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "ActionType.h"
#include "Direction.h"
#include "ThingManager.h"
#include "UsesPimpl.h"

#include "ActionImpl.h"

// Forward declarations
class Thing;

// Class describing an action to execute.
class Action
{
public:

  Action();
  Action(ActionType type);
  virtual ~Action() = default;

  void set_type(ActionType type);
  ActionType get_type() const;
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
  CopyablePimpl<ActionImpl> pImpl;
};

#endif // ACTION_H
