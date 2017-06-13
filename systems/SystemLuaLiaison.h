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

    LuaLiaison(Lua& lua,
               GameState& gameState,
               Systems::Manager& systems);

    virtual ~LuaLiaison();

    /// Recalculate whatever needs recalculating.
    void doCycleUpdate();

  protected:
    virtual void setMap_V(MapID newMap) override;

    virtual bool onEvent(Event const& event) override;

  private:
    /// Reference to the Lua object.
    Lua& m_lua;

    /// Reference to the GameState object.
    GameState& m_gameState;

    /// Reference to the Systems manager.
    /// @note Normally one system referencing another directly would be taboo,
    ///       but LuaLiaison is a little different since it requires static
    ///       pointers in place for the Lua functions to use.
    Manager& m_systems;
  };

} // end namespace Systems
