#include "things/LightOrb.h"

LightOrb::LightOrb(UUID id) : Thing("light_orb", id)
{
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
