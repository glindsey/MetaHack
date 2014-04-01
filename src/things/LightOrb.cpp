#include "LightOrb.h"

LightOrb LightOrb::prototype;

LightOrb::LightOrb() : LightSource(0)
{
  set_single_mass(2);
}

LightOrb::~LightOrb() {}

std::string LightOrb::_get_description() const
{
  return "test lighting orb";
}

sf::Vector2u LightOrb::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(1, 8);
}
