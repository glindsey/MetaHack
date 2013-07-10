#ifndef SCONCE_H
#define SCONCE_H

#include "LightSource.h"

#include "Cloneable.h"
#include "Creatable.h"
#include "ThingRegistrator.h"

/// A mounted torch.
class Sconce :
  public LightSource,
  public ThingRegistrator<Sconce>,
  public Creatable<Thing, Sconce>,
  public Cloneable<Thing, Sconce>
{
  friend class Creatable;
  friend class Cloneable;

  public:
    virtual ~Sconce();

    // Thing overrides
    virtual std::string get_description() const override;
    virtual sf::Vector2u get_tile_sheet_coords(int frame) const override;

    virtual bool do_process() override;

  protected:
    Sconce();

  private:
    static Sconce prototype;
};

#endif // LIGHTORB_H
