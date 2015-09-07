#include "things/Sconce.h"

Sconce::Sconce(UUID id) : Thing("sconce", id)
{
  set_light_color(sf::Color(160, 128, 112, 255));
  set_max_light_strength(48);
}

Sconce::~Sconce() {}

std::string Sconce::_get_description() const
{
  return "torch sconce";
}

sf::Vector2u Sconce::get_tile_sheet_coords(int frame) const
{
  if (!this->is_lit())
  {
    return sf::Vector2u(0, 7);
  }
  else
  {
    return sf::Vector2u(1 + (frame % 3), 7);
  }
}

bool Sconce::_process()
{
  /// @todo: Sconces should eventually wear out or something.
  return true;
}
