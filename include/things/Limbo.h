#ifndef LIMBO_H
#define LIMBO_H

#include <memory>

#include "Thing.h"

#include "Creatable.h"
#include "ThingRegistrator.h"

/// Forward declarations
class ThingFactory;

/// Limbo represents nothing or nowhere.  There should only be one Limbo in the
/// ThingFactory.
class Limbo :
  public Thing,
  public ThingRegistrator<Limbo>,
  public Creatable<Thing, Limbo>
{
  friend class Creatable;

  public:
    virtual ~Limbo();

    virtual std::string get_description() const override;

  protected:
    Limbo();

  private:
};

#endif // LIMBO_H
