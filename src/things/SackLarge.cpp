#include "things/SackLarge.h"

SackLarge SackLarge::prototype;

SackLarge::SackLarge() : Thing(-1)
{
  this->set_inventory_size(10);
}

SackLarge::~SackLarge()           {}

std::string SackLarge::_get_description() const
{
  return "large sack";
}

sf::Vector2u SackLarge::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(2, 8);
}
