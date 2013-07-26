#ifndef CREATABLETHING_H
#define CREATABLETHING_H

#include "ErrorHandler.h"
#include "ThingFactory.h"

class Thing;

/// CRTP template class to create subclass instances of Thing.
template <typename Subclass>
class CreatableThing
{
  friend class ThingFactory;

  public:
    CreatableThing()
    {
      static bool registered = false;
      if (!registered)
      {
        ThingFactory& factory = ThingFactory::instance();
        factory.register_class<Subclass>();
        registered = true;
      }
    }

  private:
    static Thing* create()
    {
      return new Subclass();
    }

    virtual Thing* clone() const
    {
      Subclass const* subclass = dynamic_cast<Subclass const*>(this);
      ASSERT_NOT_NULL(subclass);
      return new Subclass(*subclass);
    }
};

#endif // CLONEABLE_H
