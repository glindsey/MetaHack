#include "systems/SystemGrimReaper.h"

SystemGrimReaper::SystemGrimReaper(ComponentGlobals & globals) :
  SystemCRTP<SystemGrimReaper>({ EventEntityDied::id,
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

