#include "systems/SystemGrimReaper.h"

namespace Systems
{

  SystemGrimReaper::SystemGrimReaper(ComponentGlobals & globals) :
    CRTP<SystemGrimReaper>({ EventEntityDied::id,
                                   EventEntityMarkedForDeath::id }),
    m_globals{ globals }
  {}

  SystemGrimReaper::~SystemGrimReaper()
  {}

  void SystemGrimReaper::doCycleUpdate()
  {
  }

  void SystemGrimReaper::setMap_V(MapID newMap)
  {}

  bool SystemGrimReaper::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems