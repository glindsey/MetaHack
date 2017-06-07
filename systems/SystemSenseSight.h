#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

// Forward declarations
class ComponentInventory;
class ComponentPosition;
class ComponentSenseSight;
class ComponentSpacialMemory;
class GameState;

namespace Systems
{

  /// System that handles entity sight, and the memory of that sight.
  class SystemSenseSight : public CRTP<SystemSenseSight>
  {
  public:
    SystemSenseSight(GameState const& gameState,
                     ComponentMap<ComponentInventory> const& inventory,
                     ComponentMap<ComponentPosition> const& position,
                     ComponentMap<ComponentSenseSight>& senseSight,
                     ComponentMap<ComponentSpacialMemory>& spacialMemory);

    virtual ~SystemSenseSight();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

    /// Returns true if first entity can see second.
    bool subjectCanSeeCoords(EntityId subject, IntVec2 coords) const;

  protected:
    void findSeenTiles(EntityId id);

    void calculateRecursiveVisibility(EntityId id,
                                      ComponentPosition const& thisPosition,
                                      int octant,
                                      int depth = 1,
                                      float slope_A = 1,
                                      float slope_B = 0);

    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const & event) override;

  private:
    GameState const& m_gameState;

    // Components used by this system.
    ComponentMap<ComponentInventory> const& m_inventory;
    ComponentMap<ComponentPosition> const& m_position;
    ComponentMap<ComponentSenseSight>& m_senseSight;
    ComponentMap<ComponentSpacialMemory>& m_spacialMemory;

    /// Set of entities to update on the next cycle.
    std::set<EntityId> m_needsUpdate;
  };

} // end namespace Systems
