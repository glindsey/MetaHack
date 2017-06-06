#pragma once

#include "components/ComponentGlobals.h"
#include "entity/EntityId.h"
#include "systems/SystemCRTP.h"

/// System that handles the game clock.
class SystemTimekeeper : public SystemCRTP<SystemTimekeeper>
{
public:
  struct EventClockChanged : public ConcreteEvent<EventClockChanged>
  {
    EventClockChanged(ElapsedTicks ticks_) :
      ticks{ ticks_ }
    {}

    ElapsedTicks const ticks;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | current time: " << ticks;
    }
  };

  SystemTimekeeper(ComponentGlobals& globals);

  virtual ~SystemTimekeeper();

  /// Recalculate whatever needs recalculating.
  void doCycleUpdate();

  ElapsedTicks clock() const;

  void setClock(ElapsedTicks clock);

  void incrementClock(ElapsedTicks addedTime);

protected:
  virtual void setMap_V(MapID newMap) override;

  virtual bool onEvent(Event const& event) override;

private:
  // Components used by this system.
  ComponentGlobals& m_globals;

  ElapsedTicks m_clock;
};