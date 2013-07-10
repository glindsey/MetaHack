#ifndef ROCK_H
#define ROCK_H

#include "Thing.h"

#include "Cloneable.h"
#include "Creatable.h"
#include "ThingRegistrator.h"

/// Forward declarations
class ThingFactory;

class Rock :
  public Thing,
  public ThingRegistrator<Rock>,
  public Cloneable<Thing, Rock>,
  public Creatable<Thing, Rock>
{
  friend class Creatable;
  friend class Cloneable;

  public:
    virtual ~Rock();

    // Thing overrides
    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    Rock();

  private:
    static Rock prototype;
};

#endif // ROCK_H
