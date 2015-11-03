#include "Action.h"
#include "App.h"
#include "Direction.h"

#include <algorithm>

struct Action::Impl
{
  /// Type of this action.
  Action::Type type;

  /// Thing(s) to perform the action on.
  std::vector<ThingRef> things;

  /// If true, action can take a Thing as a target.
  bool target_can_be_thing;

  /// If true, action can take a Direction as a target.
  bool target_can_be_direction;

  /// Target Thing for the action (if any).
  ThingRef target_thing;

  /// Direction for the action (if any).
  Direction target_direction;

  /// Quantity for the action (only used in drop/pickup).
  unsigned int quantity;
};

Action::Action()
  : pImpl { NEW Impl() }
{
  pImpl->type = Type::None;
  pImpl->things.clear();
  pImpl->target_can_be_direction = false;
  pImpl->target_can_be_thing = false;
  pImpl->target_thing = TM.get_mu();
  pImpl->target_direction = Direction::None;
}

Action::Action(Action::Type type)
  : pImpl { NEW Impl() }
{
  pImpl->type = type;
  pImpl->things.clear();
  pImpl->target_thing = TM.get_mu();
  pImpl->target_direction = Direction::None;
}

Action::~Action()
{
  pImpl.reset();
}

/// Copy constructor
Action::Action(const Action& other)
  : pImpl{ NEW Impl() }
{
  *(pImpl.get()) = *(other.pImpl.get());
}

/// Move constructor
Action::Action(Action&& other) noexcept
{
  // Point our pImpl to the other's data.
  pImpl.reset(other.pImpl.get());
  // Release the other's pImpl.
  other.pImpl.release();
}

/// Copy assignment operator
Action& Action::operator= (const Action& other)
{
  Action temp(other);       // re-use copy constructor
  *this = std::move(temp);  // re-use move constructor
  return *this;
}

/// Move assignment operator
Action& Action::operator= (Action&& other) noexcept
{
  std::swap(pImpl, other.pImpl);
  return *this;
}

void Action::set_type(Action::Type type)
{
  pImpl->type = type;
}

Action::Type Action::get_type() const
{
  return pImpl->type;
}

void Action::set_things(std::vector<ThingRef> things)
{
  pImpl->things = things;
}

std::vector<ThingRef> const& Action::get_things() const
{
  return pImpl->things;
}

void Action::add_thing(ThingRef thing)
{
  pImpl->things.push_back(thing);
}

bool Action::remove_thing(ThingRef thing)
{
  auto& things = pImpl->things;
  auto& iter = std::find(things.begin(), things.end(), thing);
  if (iter != things.end())
  {
    things.erase(iter);
    return true;
  }
  return false;
}

bool Action::target_can_be_thing() const
{
  switch (pImpl->type)
  {
  case Action::Type::Fill:
  case Action::Type::PutInto:
    return true;

  default:
    return false;
  }
}

bool Action::target_can_be_direction() const
{
  switch (pImpl->type)
  {
  case Action::Type::Attack:
  case Action::Type::AttackSafe:
  case Action::Type::Close:
  case Action::Type::Fill:
  case Action::Type::Hurl:
  case Action::Type::Move:
  case Action::Type::Open:
  case Action::Type::Shoot:
    return true;

  default:
    return false;
  }
}

void Action::set_target(ThingRef target)
{
  pImpl->target_thing = target;
  pImpl->target_direction = Direction::None;
}

void Action::set_target(Direction direction)
{
  pImpl->target_thing = ThingRef();
  pImpl->target_direction = direction;
}

ThingRef const& Action::get_target_thing() const
{
  return pImpl->target_thing;
}

Direction const& Action::get_target_direction() const
{
  return pImpl->target_direction;
}

unsigned int Action::get_quantity() const
{
  return pImpl->quantity;
}

void Action::set_quantity(unsigned int quantity)
{
  pImpl->quantity = quantity;
}
