#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

namespace Systems
{

  /// System that handles entity death.
  class GrimReaper : public CRTP<GrimReaper>
  {
  public:
    struct EventEntityDied : public ConcreteEvent<EventEntityDied>
    {
      EventEntityDied(EntityId entity) :
        m_entity{ entity }
      {}

      EntityId const m_entity;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | entity = " << m_entity;
      }
    };

    struct EventEntityMarkedForDeath : public ConcreteEvent<EventEntityMarkedForDeath>
    {
      EventEntityMarkedForDeath(EntityId entity) :
        m_entity{ entity }
      {}

      EntityId const m_entity;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << " | entity = " << m_entity;
      }
    };

    GrimReaper(ComponentGlobals& globals);

    virtual ~GrimReaper();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
    ComponentGlobals& m_globals;
  };

} // end namespace Systems