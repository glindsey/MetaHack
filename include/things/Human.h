#ifndef HUMAN_H
#define HUMAN_H

#include "Biped.h"
#include "Cloneable.h"
#include "Creatable.h"
#include "ThingRegistrator.h"

class Human :
  public Biped,
  public ThingRegistrator<Human>,
  public Cloneable<Thing, Human>,
  public Creatable<Thing, Human>
{
  friend class Cloneable;
  friend class Creatable;

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
