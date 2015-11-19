#include "Action.h"
#include "App.h"
#include "Direction.h"

#include <algorithm>

Action::Action()
  :
  m_type{ Action::Type::None },
  m_state{ Action::State::Pending },
  m_things{ std::vector<ThingRef>{ } },
  m_target_thing{ ThingManager::get_mu() },
  m_target_direction{ Direction::None },
  m_quantity{ 0 }
{}

Action::Action(Action::Type type)
  :
  m_type{ type },
  m_state{ Action::State::Pending },
  m_things{ std::vector<ThingRef>{ } },
  m_target_thing{ ThingManager::get_mu() },
  m_target_direction{ Direction::None },
  m_quantity{ 0 }
{}

void Action::set_type(Action::Type type)
{
  m_type = type;
}

Action::Type Action::get_type() const
{
  return m_type;
}

void Action::set_things(std::vector<ThingRef> things)
{
  m_things = things;
}

std::vector<ThingRef> const& Action::get_things() const
{
  return m_things;
}

void Action::add_thing(ThingRef thing)
{
  m_things.push_back(thing);
}

bool Action::remove_thing(ThingRef thing)
{
  auto& things = m_things;
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
  switch (m_type)
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
  switch (m_type)
  {
  case Action::Type::Attack:    
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
  m_target_thing = target;
  m_target_direction = Direction::None;
}

void Action::set_target(Direction direction)
{
  m_target_thing = ThingRef();
  m_target_direction = direction;
}

ThingRef const& Action::get_target_thing() const
{
  return m_target_thing;
}

Direction const& Action::get_target_direction() const
{
  return m_target_direction;
}

unsigned int Action::get_quantity() const
{
  return m_quantity;
}

void Action::set_quantity(unsigned int quantity)
{
  m_quantity = quantity;
}
