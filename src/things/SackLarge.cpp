#include "SackLarge.h"

SackLarge SackLarge::prototype;

SackLarge::SackLarge() : Thing()  {}
SackLarge::~SackLarge()           {}

std::string SackLarge::get_description() const
{
  return "large sack";
}

sf::Vector2u SackLarge::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(2, 8);
}

int SackLarge::get_inventory_size() const
{
  return 5;
}
