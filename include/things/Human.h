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

    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    Human();

  private:
    static Human prototype;
};

#endif // HUMAN_H
