#ifndef ACTION_H
#define ACTION_H

#include <memory>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "Direction.h"
#include "ThingManager.h"

// Forward declarations
class Thing;

// Class describing an action to execute.
class Action
{
public:
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
  };

  Action();
  Action(Action::Type type);
  virtual ~Action();
  Action(const Action& other);
  Action(Action&& other) noexcept;
  Action& operator= (const Action& other);
  Action& operator= (Action&& other) noexcept;

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
  struct Impl;
  std::unique_ptr<Impl> pImpl;
};

#endif // ACTION_H
