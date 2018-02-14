#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

namespace Systems
{

  /// System that handles fluid mechanics.
  class Fluidics : public CRTP<Fluidics>
  {
  public:
    Fluidics();

    virtual ~Fluidics();

    /// Recalculate whatever needs recalculating.
    virtual void doCycleUpdate() override;

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
  };

} // end namespace Systems
