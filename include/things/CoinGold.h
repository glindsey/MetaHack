#ifndef COINGOLD_H
#define COINGOLD_H

#include "Aggregate.h"

#include <string>
#include <SFML/Graphics.hpp>

#include "CreatableThing.h"

class CoinGold :
  public Aggregate,
  public CreatableThing<CoinGold>
{
  friend class CreatableThing;

  public:
    virtual ~CoinGold();

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    CoinGold();

  private:
    virtual std::string _get_description() const override;
    static CoinGold prototype;
};

#endif // COINGOLD_H
