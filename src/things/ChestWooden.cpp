#include "things/ChestWooden.h"

ChestWooden ChestWooden::prototype;

ChestWooden::ChestWooden() : Thing(-1)
{
  this->set_open(false);
}

ChestWooden::~ChestWooden()           {}

std::string ChestWooden::_get_description() const
{
  return "wooden chest";
}

sf::Vector2u ChestWooden::get_tile_sheet_coords(int frame) const
{
  if (this->is_open())
  {
    return sf::Vector2u(5, 9);
  }
  else
  {
    return sf::Vector2u(6, 9);
  }
}

bool ChestWooden::is_openable() const
{
  return true;
}

bool ChestWooden::is_lockable() const
{
  return true;
}

bool ChestWooden::is_flammable() const
{
  return true;
}
