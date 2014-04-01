#ifndef REGISTERED_H
#define REGISTERED_H

#include "ErrorHandler.h"
#include "ThingFactory.h"

class Thing;

/// CRTP template class to register Things by class name with the factory.
template <typename Subclass>
class Registered
{
  friend class ThingFactory;

  public:
    Registered()
    {
      static bool registered = false;
      if (!registered)
      {
        ThingFactory& factory = ThingFactory::instance();
        factory.register_class<Subclass>();
        registered = true;
      }
    }

    static std::shared_ptr<Subclass> create()
    {
      return std::make_shared<Subclass>();
    }

    static std::shared_ptr<Thing> create_thing()
    {
      return std::static_pointer_cast<Thing>(std::make_shared<Subclass>());
    }

    virtual std::shared_ptr<Thing> clone()
    {
      return std::make_shared<Subclass>(static_cast<Subclass const&>(*this));
    }
};

#endif // REGISTERED_H
