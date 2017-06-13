#include "stdafx.h"

#include "systems/SystemThermodynamics.h"

namespace Systems
{

  Thermodynamics::Thermodynamics() :
    CRTP<Thermodynamics>({})
  {}

  Thermodynamics::~Thermodynamics()
  {}

  void Thermodynamics::doCycleUpdate()
  {}

  void Thermodynamics::setMap_V(MapID newMap)
  {}

  bool Thermodynamics::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
