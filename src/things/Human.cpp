#include "things/Human.h"

#include "App.h"

Human::Human(UUID id) : Biped("human", id)
{
  set_proper_name("John Doe");
}

Human::~Human() {}

std::string Human::_get_description() const
{
  /// @todo Make this more descriptive based on character features.
  return "human";
}

sf::Vector2u Human::get_tile_sheet_coords(int frame) const
{
  /// @todo Male/female variants; maybe skin color too?
  if (get_attributes().get(Attribute::HP) > 0)
  {
    return sf::Vector2u(0, 5);
  }
  else
  {
    return sf::Vector2u(1, 5);
  }
}
