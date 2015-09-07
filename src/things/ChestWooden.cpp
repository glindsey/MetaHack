#include "things/ChestWooden.h"

ChestWooden::ChestWooden(UUID id) : Thing("wooden_chest", id)
{
}

ChestWooden::~ChestWooden()           {}

std::string ChestWooden::_get_description() const
{
  return "wooden chest";
}

sf::Vector2u ChestWooden::get_tile_sheet_coords(int frame) const
{
  if (this->get_property_flag("open"))
  {
    return sf::Vector2u(5, 9);
  }
  else
  {
    return sf::Vector2u(6, 9);
  }
}