#pragma once

#include "lua/LuaObject.h"

// Forward declarations
class GameState;

class LuaEntityFunctions
{
public:
  LuaEntityFunctions(GameState& gameState);
  ~LuaEntityFunctions();

  static GameState* gameState();

protected:
private:
  static GameState* s_gameState;

};