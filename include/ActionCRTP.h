#ifndef ACTIONCRTP_H
#define ACTIONCRTP_H

#include "ThingRef.h"

#include <memory>

/// Template class implementing the Curiously Recurring Template Pattern
/// to present several create() functions.
template <class Derived>
class ActionCRTP
{
public:

  /// Implement a create() function that will be registerable with a factory class.
  static std::unique_ptr<Derived> create(ThingRef subject)
  {
    std::unique_ptr<Derived> action{ new Derived{ subject } };

    return std::move(action);
  }
};

#endif // ACTIONCRTP_H
