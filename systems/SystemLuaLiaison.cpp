#include "stdafx.h"

#include "systems/SystemLuaLiaison.h"

#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "systems/Manager.h"

namespace Systems
{

  LuaLiaison::LuaLiaison(Lua& lua,
                         GameState& gameState,
                         Systems::Manager& systems) :
    CRTP<LuaLiaison>({}),
    m_lua{ lua },
    m_gameState{ gameState },
    m_systems{ systems }
  {

  }

  LuaLiaison::~LuaLiaison()
  {}

  void LuaLiaison::doCycleUpdate()
  {}

  void LuaLiaison::setMap_V(MapID newMap)
  {}

  bool LuaLiaison::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
