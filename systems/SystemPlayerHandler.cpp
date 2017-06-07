#include "systems/SystemPlayerHandler.h"

namespace Systems
{

  SystemPlayerHandler::SystemPlayerHandler(ComponentGlobals & globals) :
    CRTP<SystemPlayerHandler>({ EventPlayerChanged::id }),
    m_globals{ globals }
  {}

  SystemPlayerHandler::~SystemPlayerHandler()
  {}

  void SystemPlayerHandler::doCycleUpdate()
  {}

  EntityId SystemPlayerHandler::player() const
  {
    return m_globals.player();
  }

  void SystemPlayerHandler::setPlayer(EntityId entity)
  {
    Assert("PlayerHandler", entity != EntityId::Mu(), "tried to make nothingness the player");

    auto oldPlayer = m_globals.player();
    m_globals.setPlayer(entity);
    if (oldPlayer != entity)
    {
      broadcast(EventPlayerChanged(entity));
    }
  }

  void SystemPlayerHandler::setMap_V(MapID newMap)
  {}

  bool SystemPlayerHandler::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
