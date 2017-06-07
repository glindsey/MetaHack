#pragma once

#include "components/ComponentActivity.h"
#include "components/ComponentGlobals.h"
#include "components/ComponentMap.h"
#include "components/ComponentInventory.h"
#include "entity/EntityId.h"
#include "map/MapFactory.h"
#include "systems/SystemCRTP.h"

/// System that handles having entities perform actions.
class SystemDirector : public SystemCRTP<SystemDirector>
{
public:
  struct EventDummy : public ConcreteEvent<EventDummy>
  {
    EventDummy()
    {}

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | dummy";
    }
  };

  SystemDirector(GameState& gameState,
                 SystemManager& systems);

  virtual ~SystemDirector();

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

protected:
  void processMap(MapID mapID);

  void processEntityAndChildren(EntityId entityID);

  void processEntity(EntityId entityID);

  virtual void setMap_V(MapID newMap) override;

  virtual bool onEvent(Event const& event) override;

private:
  // Components used by this system.
  GameState& m_gameState;
  SystemManager& m_systems;
};