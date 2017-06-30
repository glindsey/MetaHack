#include "stdafx.h"

#include "systems/SystemChoreographer.h"

namespace Systems
{

  Choreographer::Choreographer(Components::ComponentGlobals & globals) :
    CRTP<Choreographer>({ EventPlayerChanged::id }),
    m_globals{ globals }
  {}

  Choreographer::~Choreographer()
  {}

  void Choreographer::doCycleUpdate()
  {}

  EntityId Choreographer::player() const
  {
    return m_globals.player();
  }

  void Choreographer::setPlayer(EntityId entity)
  {
    Assert("PlayerHandler", entity != EntityId::Void, "tried to make nothingness the player");

    auto oldPlayer = m_globals.player();
    m_globals.setPlayer(entity);
    if (oldPlayer != entity)
    {
      broadcast(EventPlayerChanged(entity));
    }
  }

  void Choreographer::setMap_V(MapID newMap)
  {}

  bool Choreographer::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
