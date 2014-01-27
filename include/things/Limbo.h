#ifndef LIMBO_H
#define LIMBO_H

#include <memory>

#include "Container.h"
#include "CreatableThing.h"

/// Forward declarations
class ThingFactory;

/// Limbo represents nothing or nowhere.  There should only be one Limbo in the
/// ThingFactory.
class Limbo :
  public Container,
  public CreatableThing <Limbo>
{
  friend class CreatableThing;

  public:
    virtual ~Limbo();

  protected:
    Limbo();

  private:
    virtual std::string _get_description() const override;
};

#endif // LIMBO_H
