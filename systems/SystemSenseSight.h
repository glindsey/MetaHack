#pragma once

#include "components/ComponentMap.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

// Forward declarations
namespace Components
{
  class ComponentInventory;
  class ComponentPosition;
  class ComponentSenseSight;
  class ComponentSpacialMemory;
}
class GameState;

namespace Systems
{

  /// System that handles entity sight, and the memory of that sight.
  class SenseSight : public CRTP<SenseSight>
  {
  public:
    SenseSight(GameState const& gameState,
               Components::ComponentMap<Components::ComponentInventory> const& inventory,
               Components::ComponentMap<Components::ComponentPosition> const& position,
               Components::ComponentMap<Components::ComponentSenseSight>& senseSight,
               Components::ComponentMap<Components::ComponentSpacialMemory>& spacialMemory);

    virtual ~SenseSight();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

    /// Returns true if first entity can see second.
    bool subjectCanSeeCoords(EntityId subject, IntVec2 coords) const;

  protected:
    void findSeenTiles(EntityId id);

    void calculateRecursiveVisibility(EntityId id,
                                      Components::ComponentPosition const& thisPosition,
                                      int octant,
                                      int depth = 1,
                                      float slope_A = 1,
                                      float slope_B = 0);

    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const & event) override;

  private:
    GameState const& m_gameState;

    // Components used by this system.
    Components::ComponentMap<Components::ComponentInventory> const& m_inventory;
    Components::ComponentMap<Components::ComponentPosition> const& m_position;
    Components::ComponentMap<Components::ComponentSenseSight>& m_senseSight;
    Components::ComponentMap<Components::ComponentSpacialMemory>& m_spacialMemory;

    /// Set of entities to update on the next cycle.
    std::set<EntityId> m_needsUpdate;
  };

} // end namespace Systems
