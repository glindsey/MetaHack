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

      void printToStream(std::ostream& os) const
      {
        Event::printToStream(os);
        os << " | entity = " << m_entity;
      }
    };

    struct EventEntityMarkedForDeath : public ConcreteEvent<EventEntityMarkedForDeath>
    {
      EventEntityMarkedForDeath(EntityId entity) :
        m_entity{ entity }
      {}

      EntityId const m_entity;

      void printToStream(std::ostream& os) const
      {
        Event::printToStream(os);
        os << " | entity = " << m_entity;
      }
    };

    GrimReaper(Components::ComponentGlobals& globals);

    virtual ~GrimReaper();

    /// Recalculate whatever needs recalculating.
    virtual void doCycleUpdate() override;

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
    Components::ComponentGlobals& m_globals;
  };

} // end namespace Systems
