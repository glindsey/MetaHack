#include "things/RockLichen.h"

#include "App.h"

RockLichen::RockLichen(UUID id) : Blob("rock lichen", id)
{
  //ctor
}

RockLichen::~RockLichen()
{
  //dtor
}

std::string RockLichen::_get_description() const
{
  return "rock lichen";
}

sf::Vector2u RockLichen::get_tile_sheet_coords(int frame) const
{
  return sf::Vector2u(4, 5);
}
