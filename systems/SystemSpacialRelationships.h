#pragma once

#include "components/ComponentMap.h"
#include "components/ComponentPosition.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

// Forward declarations
class ComponentGlobals;
class ComponentInventory;

namespace Systems
{

  /// System that handles spacial relationships between entities -- what's inside
  /// what, what's adjacent to what, moving entities, et cetera.
  class SystemSpacialRelationships : public CRTP<SystemSpacialRelationships>
  {
  public:
    /// Event indicating an entity moved, but stayed on the same map.
    /// 
    struct EventEntityMoved : public ConcreteEvent<EventEntityMoved>
    {
      EventEntityMoved(EntityId entity_, ComponentPosition const& oldPosition_) :
        entity{ entity_ }, oldPosition{ oldPosition_ }
      {
      }

      EntityId const entity;
      ComponentPosition const oldPosition;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << "| entity = " << entity << " | old position = " << oldPosition;
      }
    };

    /// Event indicating an entity moved to a new map.
    struct EventEntityChangedMaps : public ConcreteEvent<EventEntityChangedMaps>
    {
      EventEntityChangedMaps(EntityId entity_) :
        entity{ entity_ }
      {
      }

      EntityId const entity;

      void serialize(std::ostream& os) const
      {
        Event::serialize(os);
        os << "| entity = " << entity;
      }
    };

    SystemSpacialRelationships(ComponentGlobals const& globals,
                               ComponentMap<ComponentInventory>& inventory,
                               ComponentMap<ComponentPosition>& position);

    virtual ~SystemSpacialRelationships();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

    /// Move an entity into a location, if possible.
    bool moveEntityInto(EntityId entity, EntityId newLocation);

    /// Return whether the first entity can reach the second.
    /// "Can reach" means they are:
    ///   - at the same location
    ///   - at adjacent locations
    ///   - second is child of first
    bool firstCanReachSecond(EntityId first, EntityId second) const;

    /// Return whether entity is inside another entity.
    bool insideAnotherEntity(EntityId entity) const;

    /// Return whether entity is *directly* inside another entity (e.g. must be
    /// a direct child, not a descendant.)
    bool firstIsDirectlyInsideSecond(EntityId first, EntityId second) const;

    /// Return whether the first entity is inside the second.
    bool firstIsInsideSecond(EntityId first, EntityId second) const;

    /// Return whether these two entities are adjacent to each other.
    bool areAdjacent(EntityId first, EntityId second) const;

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
    ComponentGlobals const& m_globals;
    ComponentMap<ComponentInventory>& m_inventory;
    ComponentMap<ComponentPosition>& m_position;
  };

} // end namespace Systems
