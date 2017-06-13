#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

namespace Systems
{

  /// System that handles thermodynamics.
  class Thermodynamics : public CRTP<Thermodynamics>
  {
  public:
    Thermodynamics();

    virtual ~Thermodynamics();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
  };

} // end namespace Systems
