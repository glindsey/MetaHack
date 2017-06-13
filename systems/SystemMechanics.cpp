#include "stdafx.h"

#include "systems/SystemMechanics.h"

namespace Systems
{

  Mechanics::Mechanics() :
    CRTP<Mechanics>({})
  {}

  Mechanics::~Mechanics()
  {}

  void Mechanics::doCycleUpdate()
  {}

  void Mechanics::setMap_V(MapID newMap)
  {}

  bool Mechanics::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
