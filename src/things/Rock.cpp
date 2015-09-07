#include "things/Rock.h"

Rock::Rock(UUID id) : Thing("rock", id)
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
