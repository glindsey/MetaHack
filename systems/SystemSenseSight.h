#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "map/MapId.h"
#include "systems/SystemCRTP.h"

// Forward declarations
class ComponentInventory;
class ComponentPosition;
class ComponentSenseSight;

/// System that handles entity sight.
class SystemSenseSight : public SystemCRTP<SystemSenseSight>
{
public:
  SystemSenseSight(ComponentMap<ComponentInventory>& inventory,
                   ComponentMap<ComponentPosition>& position,
                   ComponentMap<ComponentSenseSight>& senseSight);

  virtual ~SystemSenseSight();

  /// Recalculate whatever needs recalculating.
  void recalculate();

protected:
  void setMapNVO(MapId newMap);

private:
  // Components used by this system.
  ComponentMap<ComponentInventory>& m_inventory;
  ComponentMap<ComponentPosition>& m_position;
  ComponentMap<ComponentSenseSight>& m_senseSight;
};