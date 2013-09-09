#include "CoinGold.h"

CoinGold CoinGold::prototype;

CoinGold::CoinGold()
{
  //ctor
}

CoinGold::~CoinGold()
{
  //dtor
}

std::string CoinGold::get_description() const
{
  return "gold coin";
}

sf::Vector2u CoinGold::get_tile_sheet_coords(int frame) const
{
  unsigned int quantity = this->get_quantity();

  if (quantity < 10)
  {
    return sf::Vector2u(0, 15);
  }
  else if (quantity < 100)
  {
    return sf::Vector2u(1, 15);
  }
  else
  {
    return sf::Vector2u(2, 15);
  }
}
