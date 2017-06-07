#include "systems/SystemTimekeeper.h"

namespace Systems
{

  Timekeeper::Timekeeper(ComponentGlobals & globals) :
    CRTP<Timekeeper>({ EventClockChanged::id }),
    m_globals{ globals }
  {}

  Timekeeper::~Timekeeper()
  {}

  void Timekeeper::doCycleUpdate()
  {
    incrementClock(ElapsedTicks(1));
  }

  ElapsedTicks Timekeeper::clock() const
  {
    return m_clock;
  }

  void Timekeeper::setClock(ElapsedTicks clock)
  {
    m_clock = clock;
    broadcast(EventClockChanged(clock));
  }

  void Timekeeper::incrementClock(ElapsedTicks addedTime)
  {
    /// @todo Check for the unlikely, but not impossible, chance of rollover.
    setClock(clock() + addedTime);
  }

  void Timekeeper::setMap_V(MapID newMap)
  {}

  bool Timekeeper::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
