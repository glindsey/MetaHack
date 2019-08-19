#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/CRTP.h"

namespace Systems
{

  /// System that handles the game clock.
  class Timekeeper : public CRTP<Timekeeper>
  {
  public:
    struct EventClockChanged : public ConcreteEvent<EventClockChanged>
    {
      EventClockChanged(ElapsedTicks ticks_) :
        ticks{ ticks_ }
      {}

      ElapsedTicks const ticks;

      void printToStream(std::ostream& os) const
      {
        Event::printToStream(os);
        os << " | current time: " << ticks;
      }
    };

    Timekeeper(Components::ComponentGlobals& globals);

    virtual ~Timekeeper();

    /// Recalculate whatever needs recalculating.
    virtual void doCycleUpdate() override;

    ElapsedTicks clock() const;

    void setClock(ElapsedTicks clock);

    void incrementClock(ElapsedTicks addedTime);

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    // Components used by this system.
    Components::ComponentGlobals& m_globals;

    ElapsedTicks m_clock;
  };

} // end namespace Systems
