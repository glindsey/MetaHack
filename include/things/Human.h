#ifndef HUMAN_H
#define HUMAN_H

#include "Biped.h"
#include "CreatableThing.h"

class Human :
  public Biped,
  public CreatableThing<Human>
{
  friend class CreatableThing;

  public:
    virtual ~Human();

    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    Human();

  private:
    virtual std::string _get_description() const override;
    static Human prototype;
};

#endif // HUMAN_H
