#pragma once

#include "systems/Base.h"

namespace Systems
{

  /// CRTP class for all systems in the game.
  template <class Subclass>
  class CRTP : public Base
  {
  public:
    CRTP(std::unordered_set<EventID> const events) :
      Base(events)
    {}

    virtual ~CRTP() {}

    /// Get this system, cast to the appropriate subclass.
    Subclass* operator->() { return static_cast<Subclass*>(this); }

  protected:

  private:
  };

} // end namespace Systems