#pragma once

#include "Action.h"
#include "entity/EntityId.h"

namespace Actions
{
  /// Template class implementing the Curiously Recurring Template Pattern
  /// to present several create() functions.
  template <class Derived>
  class ActionCRTP
  {
  public:

    /// Implement a create_() function that will be registerable with a factory class.
    static std::unique_ptr<Action> create_(EntityId subject)
    {
      std::unique_ptr<Action> action{ new Derived{ subject } };

        return action;
    }
  };
}
