#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/SystemCRTP.h"

/// System that handles entity death, as well as entity destruction.
class SystemGrimReaper : public SystemCRTP<SystemGrimReaper>
{
public:
  struct EventDummy : public ConcreteEvent<EventDummy>
  {
    EventDummy()
    {}

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
    }
  };

  SystemGrimReaper(ComponentGlobals& globals);

  virtual ~SystemGrimReaper();

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

protected:
  virtual void setMap_V(MapID newMap) override;

  virtual bool onEvent(Event const& event) override;

private:
  // Components used by this system.
  ComponentGlobals& m_globals;
};