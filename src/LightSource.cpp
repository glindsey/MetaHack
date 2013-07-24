#include "LightSource.h"

#include "Entity.h"

struct LightSource::Impl
{
  Direction direction;  ///< The direction the light is facing.
  int level;            ///< The current light level (when lit).
  int max_strength;     ///< How far the light reaches.
  sf::Color color;      ///< The overall "color" of the light.
  bool lit;             ///< Whether the light is lit or not.
};

LightSource::LightSource()
  : Aggregate(), impl(new Impl())
{
  impl->direction = Direction::Up;
  impl->level = 256;
  impl->color = sf::Color(64, 64, 64, 255);
  impl->max_strength = 64;
  impl->lit = true;
}

LightSource::LightSource(LightSource const& original)
  : Aggregate(), impl(new Impl())
{
  impl->direction = original.get_light_direction();
  impl->level = original.get_light_level();
  impl->color = original.get_light_color();
  impl->max_strength = original.get_max_light_strength();
  impl->lit = original.is_lit();
}

LightSource::~LightSource()
{
}

int LightSource::get_light_strength() const
{
  return (impl->lit) ?
         ((impl->max_strength * impl->level) / 256) : 0;
}

int LightSource::get_light_level() const
{
  return impl->level;
}

void LightSource::set_light_level(int level)
{
  impl->level = level;
}

int LightSource::get_max_light_strength() const
{
  return impl->max_strength;
}

void LightSource::set_max_light_strength(int max_strength)
{
  impl->max_strength = max_strength;
}

Direction LightSource::get_light_direction() const
{
  return impl->direction;
}

void LightSource::set_light_direction(Direction direction)
{
  impl->direction = direction;
}

sf::Color LightSource::get_light_color() const
{
  return impl->color;
}

void LightSource::set_light_color(sf::Color color)
{
  impl->color = color;
}

bool LightSource::usable_by(Entity const& entity) const
{
  return true;
}

bool LightSource::_perform_action_activated_by(Entity& entity)
{
  impl->lit = !(impl->lit);
  return true;
}

void LightSource::set_lit(bool lit)
{
  impl->lit = lit;
}

bool LightSource::is_lit() const
{
  return impl->lit;
}
