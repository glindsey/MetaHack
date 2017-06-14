#include "stdafx.h"

#include "systems/SystemLuaLiaison.h"

#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "systems/Manager.h"

#include "lua/LuaFunctions-Entity.h"
#include "lua/LuaFunctions-Global.h"

namespace Systems
{
  GameState* LuaLiaison::s_gameState = nullptr;
  Systems::Manager* LuaLiaison::s_systems = nullptr;

  LuaLiaison::LuaLiaison(GameState& gameState,
                         Systems::Manager& systems) :
    CRTP<LuaLiaison>({})
  {
    s_gameState = &gameState;
    s_systems = &systems;
    LuaFunctions::registerFunctionsGlobal(gameState.lua());
    LuaFunctions::registerFunctionsEntity(gameState.lua());
  }

  LuaLiaison::~LuaLiaison()
  {
    s_gameState = nullptr;
    s_systems = nullptr;
  }

  void LuaLiaison::doCycleUpdate()
  {}

  GameState & LuaLiaison::gameState()
  {
    Assert("Lua", s_gameState != nullptr, "Attempted to retrieve null GameState reference");
    return *s_gameState;
  }

  Manager & LuaLiaison::systems()
  {
    Assert("Lua", s_systems != nullptr, "Attempted to retrieve null GameState reference");
    return *s_systems;
  }

  void LuaLiaison::setMap_V(MapID newMap)
  {}

  bool LuaLiaison::onEvent(Event const & event)
  {
    return false;
  }

} // end namespace Systems
