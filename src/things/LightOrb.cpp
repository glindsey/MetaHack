#include "LightOrb.h"

LightOrb LightOrb::prototype;

LightOrb::LightOrb() :
  LightSource()
{
  set_size(2);
}

LightOrb::~LightOrb() {}

std::string LightOrb::get_description() const
{
  return "test lighting orb";
}

sf::Vector2u LightOrb::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(1, 8);
}
