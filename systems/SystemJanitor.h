#pragma once

#include "components/ComponentManager.h"
#include "entity/EntityId.h"
#include "systems/SystemCRTP.h"

/// System that handles entity cleanup (i.e. deletion).
class SystemJanitor : public SystemCRTP<SystemJanitor>
{
public:
  struct EventEntityDestroyed : public ConcreteEvent<EventEntityDestroyed>
  {
    EventEntityDestroyed(EntityId entity) :
      m_entity{ entity }
    {}

    EntityId const m_entity;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | entity = " << m_entity;
    }
  };

  struct EventEntityMarkedForDeletion : public ConcreteEvent<EventEntityMarkedForDeletion>
  {
    EventEntityMarkedForDeletion(EntityId entity) :
      m_entity{ entity }
    {}

    EntityId const m_entity;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | entity = " << m_entity;
    }
  };

  SystemJanitor(ComponentManager& components);

  virtual ~SystemJanitor();

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

  /// Mark an entity for deletion.
  void markForDeletion(EntityId id);

protected:
  virtual void setMap_V(MapID newMap) override;

  virtual bool onEvent(Event const& event) override;

private:
  /// Reference to all components.
  ComponentManager& m_components;

  /// Collection of entities that have been marked for deletion.
  std::deque<EntityId> m_entitiesPendingDeletion;
};