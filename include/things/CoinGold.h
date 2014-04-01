#ifndef COINGOLD_H
#define COINGOLD_H

#include <string>
#include <SFML/Graphics.hpp>

#include "Thing.h"
#include "Registered.h"

class CoinGold :
  public Thing,
  public Registered<CoinGold>
{
  public:
    CoinGold();
    virtual ~CoinGold();
    CoinGold(CoinGold const&) = default;
    CoinGold const& operator=(CoinGold const&) = delete;

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:


  private:
    virtual std::string _get_description() const override;
    static CoinGold prototype;
};

#endif // COINGOLD_H
