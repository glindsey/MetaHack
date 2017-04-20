#pragma once

#include "map/MapId.h"
#include "map/MapMemory.h"

/// Component that represents the entity's memory of maps.
class ComponentSpacialMemory
{
public:

  friend void from_json(json const& j, ComponentSpacialMemory& obj);
  friend void to_json(json& j, ComponentSpacialMemory const& obj);

  bool containsMap(MapId map);

  MapMemory& ofMap(MapId map);
  MapMemory const& ofMap(MapId map) const;
  MapMemory& operator[](MapId map);

protected:

private:
  /// Spacial memory of each map.
  std::unordered_map<MapId, MapMemory> m_memory;
};

