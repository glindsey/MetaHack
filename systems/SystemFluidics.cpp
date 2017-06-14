#include "stdafx.h"

#include "systems/SystemFluidics.h"

namespace Systems
{

  Fluidics::Fluidics() :
    CRTP<Fluidics>({})
  {}

  Fluidics::~Fluidics()
  {}

  void Fluidics::doCycleUpdate()
  {}

  void Fluidics::setMap_V(MapID newMap)
  {}

  bool Fluidics::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
