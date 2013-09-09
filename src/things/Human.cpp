#include "Human.h"

#include "App.h"

Human Human::prototype;

Human::Human() :
  Biped()
{
  set_proper_name("John Doe");
}

Human::~Human() {}

std::string Human::get_description() const
{
  /// @todo Make this more descriptive based on character features.
  return "human";
}

sf::Vector2u Human::get_tile_sheet_coords(int frame) const
{
  /// @todo Male/female variants; maybe skin color too?
  return sf::Vector2u(0, 5);
}
