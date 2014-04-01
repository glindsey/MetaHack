#include "Rock.h"

Rock Rock::prototype;

Rock::Rock() : Thing(0)
{
  // ctor
}

Rock::~Rock()
{
  // dtor
}

std::string Rock::_get_description() const
{
  return "small rock";
}

sf::Vector2u Rock::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(0, 8);
}
