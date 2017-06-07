#pragma once

#include "Object.h"

namespace Systems
{

  /// Superclass for all systems in the game.
  class Base : public Object
  {
  public:
    Base(std::unordered_set<EventID> const events) : Object(events) {}
    virtual ~Base() {}

    /// Get the map the system is operating on.
    virtual MapID map() const { return m_map; }

    /// Set the map the system is operating on.
    virtual void setMap(MapID map)
    {
      if (m_map != map)
      {
        setMap_V(map);
      }
      m_map = map;
    }

    /// Recalculate whatever needs recalculating.
    virtual void doCycleUpdate() = 0;

  protected:
    /// Virtual override called after the map is changed.
    virtual void setMap_V(MapID newMap) = 0;

  private:
    /// ID of map the system is operating on.
    MapID m_map;
  };

} // end namespace Systems