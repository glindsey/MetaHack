#pragma once

#include "lua/LuaObject.h"

// Forward declarations
class GameState;
namespace Systems
{
  class Manager;
}

class LuaEntityFunctions
{
public:
  LuaEntityFunctions(GameState& gameState, 
                     Systems::Manager& systems);
  ~LuaEntityFunctions();

  static GameState& gameState();
  static Systems::Manager& systems();

protected:
private:
  static GameState* s_gameState;
  static Systems::Manager* s_systemManager;

};