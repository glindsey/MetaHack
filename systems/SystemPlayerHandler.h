#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/SystemCRTP.h"

/// System that handles which entity is the player.
class SystemPlayerHandler : public SystemCRTP<SystemPlayerHandler>
{
public:
  struct EventPlayerChanged : public ConcreteEvent<EventPlayerChanged>
  {
    EventPlayerChanged(EntityId player_) :
      player{ player_ }
    {}

    EntityId const player;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | current player: " << player;
    }
  };

  SystemPlayerHandler(ComponentGlobals& globals);

  virtual ~SystemPlayerHandler();

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

  /// Get current player.
  EntityId player() const;

  /// Set current player.
  void setPlayer(EntityId entity);

protected:
  void setMapNVO(MapID newMap);

  virtual bool onEvent(Event const& event) override;

private:
  // Components used by this system.
  ComponentGlobals& m_globals;
};