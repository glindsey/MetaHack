#pragma once

#include "map/MapId.h"
#include "Object.h"

/// Superclass for all systems in the game.
class System : public Object
{
public:
  System(std::unordered_set<EventID> const events) : Object(events) {}
  virtual ~System() {}

  /// Get the map the system is operating on.
  virtual MapId map() const { return m_map; }

  /// Set the map the system is operating on.
  virtual void setMap(MapId map) 
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
  virtual void setMapNVO(MapId newMap) = 0;

private:
  /// ID of map the system is operating on.
  MapId m_map;
};