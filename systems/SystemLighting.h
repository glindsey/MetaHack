#pragma once

#include "components/ComponentMap.h"
#include "map/MapId.h"

// Forward declarations
class ComponentLightSource;
class ComponentPosition;

/// System that handles lighting the map and all entities on it.
class SystemLighting
{
public:
  SystemLighting(ComponentMap<ComponentLightSource>& lightSources,
                 ComponentMap<ComponentPosition>& positions);

  /// Get the map the system is operating on.
  MapId map() const;

  /// Set the map the system is operating on.
  void setMap(MapId map);

  /// Recalculate map lighting.
  void recalculate();

protected:

private:
  /// Reference to light source components.
  ComponentMap<ComponentLightSource>& m_lightSources;

  /// Reference to position components.
  ComponentMap<ComponentPosition>& m_positions;

  /// ID of map the system is operating on.
  MapId m_map;

};