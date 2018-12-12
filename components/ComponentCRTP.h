#pragma once

#include "ComponentBase.h"

namespace Components
{
  /// Template class implementing the Curiously Recurring Template Pattern
  /// to present several create() functions.
  template <class Subclass>
  class ComponentCRTP : public ComponentBase
  {
  public:
    friend void from_json(json const& j, ComponentCRTP& obj)
    {
      from_json(j, static_cast<Subclass&>(obj));
    }

    friend void to_json(json& j, ComponentCRTP const& obj)
    {
      to_json(j, static_cast<Subclass const&>(*obj));
    }

    /// Get this component, cast to the appropriate subclass.
    Subclass* operator->() { return static_cast<Subclass*>(this); }
  };
}
