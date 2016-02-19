#ifndef ACTIONCRTP_H
#define ACTIONCRTP_H

#include "Action.h"
#include "ThingRef.h"

#include <memory>

/// Template class implementing the Curiously Recurring Template Pattern
/// to present several create() functions.
template <class Derived>
class ActionCRTP
{
public:

  /// Implement a create_() function that will be registerable with a factory class.
  static std::unique_ptr<Action> create_(ThingRef subject)
  {
    std::unique_ptr<Action> action{ new Derived{ subject } };

    return std::move(action);
  }
};

#endif // ACTIONCRTP_H
