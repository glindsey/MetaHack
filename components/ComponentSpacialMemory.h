#pragma once

#include <unordered_map>

#include "map/MapMemory.h"
#include "types/common.h"

namespace Components
{

  /// Component that represents the entity's memory of maps.
  class ComponentSpacialMemory
  {
  public:

    friend void from_json(json const& j, ComponentSpacialMemory& obj);
    friend void to_json(json& j, ComponentSpacialMemory const& obj);

    bool containsMap(MapID map);

    MapMemory& ofMap(MapID map);
    MapMemory const& ofMap(MapID map) const;
    MapMemory& operator[](MapID map);

  protected:

  private:
    /// Spacial memory of each map.
    std::unordered_map<MapID, MapMemory> m_memory;
  };

} // end namespace Components
