#include "Action.h"
#include "App.h"
#include "Direction.h"

#include <algorithm>

Action::Action()
{
  pImpl->type = ActionType::None;
  pImpl->things.clear();
  pImpl->target_thing = TM.get_mu();
  pImpl->target_direction = Direction::None;
}

Action::Action(ActionType type)
{
  pImpl->type = type;
  pImpl->things.clear();
  pImpl->target_thing = TM.get_mu();
  pImpl->target_direction = Direction::None;
}

void Action::set_type(ActionType type)
{
  pImpl->type = type;
}

ActionType Action::get_type() const
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

// @todo Action::target_can_be_bodypart()

bool Action::target_can_be_thing() const
{
  switch (pImpl->type)
  {
  case ActionType::Fill:
  case ActionType::PutInto:
    return true;

  default:
    return false;
  }
}

bool Action::target_can_be_direction() const
{
  switch (pImpl->type)
  {
  case ActionType::Attack:    
  case ActionType::Close:
  case ActionType::Fill:
  case ActionType::Hurl:
  case ActionType::Move:
  case ActionType::Open:
  case ActionType::Shoot:
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
