#ifndef LIGHTORB_H
#define LIGHTORB_H

#include "LightSource.h"
#include "Cloneable.h"
#include "Creatable.h"
#include "ThingRegistrator.h"

/// A test object, used for testing LightSources.  The final class will just be
/// an "orb".
class LightOrb :
  public LightSource,
  public ThingRegistrator<LightOrb>,
  public Cloneable<Thing, LightOrb>,
  public Creatable<Thing, LightOrb>
{
  friend class Cloneable;
  friend class Creatable;

  public:
    virtual ~LightOrb();

    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

  protected:
    LightOrb();

  private:
    static LightOrb prototype;
};

#endif // LIGHTORB_H
