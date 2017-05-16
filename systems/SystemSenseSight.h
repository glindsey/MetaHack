#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "map/MapId.h"
#include "systems/SystemCRTP.h"

// Forward declarations
class ComponentInventory;
class ComponentPosition;
class ComponentSenseSight;
class ComponentSpacialMemory;

/// System that handles entity sight, and the memory of that sight.
class SystemSenseSight : public SystemCRTP<SystemSenseSight>
{
public:
  SystemSenseSight(ComponentMap<ComponentInventory> const& inventory,
                   ComponentMap<ComponentPosition> const& position,
                   ComponentMap<ComponentSenseSight>& senseSight,
                   ComponentMap<ComponentSpacialMemory>& spacialMemory);

  virtual ~SystemSenseSight();

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

  /// Returns true if first entity can see second.
  bool subjectCanSeeCoords(EntityId subject, IntVec2 coords) const;

protected:
  void setMapNVO(MapId newMap);

  void findSeenTiles(EntityId id);

  void calculateRecursiveVisibility(EntityId id,
                                    ComponentPosition const& thisPosition,
                                    int octant,
                                    int depth = 1,
                                    float slope_A = 1,
                                    float slope_B = 0);

  virtual bool onEvent(Event const & event) override;

private:
  // Components used by this system.
  ComponentMap<ComponentInventory> const& m_inventory;
  ComponentMap<ComponentPosition> const& m_position;
  ComponentMap<ComponentSenseSight>& m_senseSight;
  ComponentMap<ComponentSpacialMemory>& m_spacialMemory;

  /// Set of entities to update on the next cycle.
  std::set<EntityId> m_needsUpdate;
};