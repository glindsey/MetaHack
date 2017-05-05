#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "map/MapId.h"
#include "systems/SystemCRTP.h"

// Forward declarations
class ComponentInventory;
class ComponentPosition;

/// System that handles spacial relationships between entities.
class SystemSpacialRelationships : public SystemCRTP<SystemSpacialRelationships>
{
public:
  SystemSpacialRelationships(ComponentMap<ComponentInventory>& inventory,
                             ComponentMap<ComponentPosition>& position);

  virtual ~SystemSpacialRelationships();

  /// Recalculate whatever needs recalculating.
  void recalculate();

protected:
  void setMapNVO(MapId newMap);

private:
  // Components used by this system.
  ComponentMap<ComponentInventory>& m_inventory;
  ComponentMap<ComponentPosition>& m_position;

  /// ID of map the system is operating on.
  MapId m_map;
};