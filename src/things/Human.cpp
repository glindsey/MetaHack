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
  // TODO: make this more descriptive
  return "human";
}

sf::Vector2u Human::get_tile_sheet_coords(int frame) const
{
  // TODO: male/female variants
  int x_pos = get_appropriate_4way_tile(this->get_facing_direction());
  return sf::Vector2u(x_pos, 5);
}
