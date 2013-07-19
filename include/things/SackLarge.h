#ifndef SACK_LARGE_H
#define SACK_LARGE_H

#include "Container.h"
#include "Cloneable.h"
#include "Creatable.h"
#include "ThingRegistrator.h"

/// Forward declarations
class ThingFactory;

class SackLarge :
  public Container,
  public ThingRegistrator<SackLarge>,
  public Cloneable<Thing, SackLarge>,
  public Creatable<Thing, SackLarge>
{
  friend class Cloneable;
  friend class Creatable;

  public:
    virtual ~SackLarge();

    // Thing overrides
    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    SackLarge();

  private:
    static SackLarge prototype;
};

#endif // ROCK_H
