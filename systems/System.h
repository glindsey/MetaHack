#pragma once

#include "Object.h"

/// Superclass for all systems in the game.
class System : public Object
{
public:
  System(std::unordered_set<EventID> const events) : Object(events) {}
  virtual ~System() {}

  /// Get the map the system is operating on.
  virtual MapID map() const { return m_map; }

  /// Set the map the system is operating on.
  virtual void setMap(MapID map) 
  {
    if (m_map != map)
    {
      setMapNVO(map);
    }
    m_map = map;
  }

  /// Recalculate whatever needs recalculating.
  virtual void doCycleUpdate() = 0;

protected:
  /// Virtual override called after the map is changed.
  virtual void setMapNVO(MapID newMap) = 0;

private:
  /// ID of map the system is operating on.
  MapID m_map;
};