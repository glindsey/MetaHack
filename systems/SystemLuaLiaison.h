#pragma once

#include "systems/CRTP.h"
#include "types/common.h"

// Forward declarations
class GameState;
class Lua;
namespace Systems
{
  class Manager;
}

namespace Systems
{

  /// System that handles registering functions for Lua scripts to call.
  class LuaLiaison : public CRTP<LuaLiaison>
  {
  public:

    LuaLiaison(GameState& gameState,
               Systems::Manager& systems);

    virtual ~LuaLiaison();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

    /// Get the static GameState reference.
    static GameState& gameState();

    /// Get the static SystemManager reference.
    static Manager& systems();

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    /// Pointer to the GameState object.
    static GameState* s_gameState;

    /// Pointer to the Systems manager.
    static Manager* s_systems;
  };

} // end namespace Systems
