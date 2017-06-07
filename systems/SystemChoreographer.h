#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

namespace Systems
{

  /// System that handles which entity is the player.
  class Choreographer : public CRTP<Choreographer>
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

    Choreographer(Components::ComponentGlobals& globals);

    virtual ~Choreographer();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

    /// Get current player.
    EntityId player() const;

    /// Set current player.
    void setPlayer(EntityId entity);

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
    Components::ComponentGlobals& m_globals;
  };

} // end namespace Systems
