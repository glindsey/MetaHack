#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "map/MapId.h"
#include "systems/SystemCRTP.h"

// Forward declarations
class ComponentInventory;
class ComponentPosition;

/// System that handles spacial relationships between entities -- what's inside
/// what, what's adjacent to what, moving entities, et cetera.
class SystemSpacialRelationships : public SystemCRTP<SystemSpacialRelationships>
{
public:
  SystemSpacialRelationships(ComponentMap<ComponentInventory>& inventory,
                             ComponentMap<ComponentPosition>& position);

  virtual ~SystemSpacialRelationships();

  /// Recalculate whatever needs recalculating.
  void recalculate();

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
  void setMapNVO(MapId newMap);

private:
  // Components used by this system.
  ComponentMap<ComponentInventory>& m_inventory;
  ComponentMap<ComponentPosition>& m_position;

  /// ID of map the system is operating on.
  MapId m_map;
};