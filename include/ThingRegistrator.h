#ifndef THINGREGISTRATOR_H_INCLUDED
#define THINGREGISTRATOR_H_INCLUDED

#include "ThingFactory.h"

template<typename Derived>
class ThingRegistrator
{
public:
  ThingRegistrator()
  {
    static bool registered = false;
    if (!registered)
    {
      ThingFactory& f = ThingFactory::instance();
      f.register_class<Derived>();
      registered = true;
    }
  }
};

#endif // THINGREGISTRATOR_H_INCLUDED
