#pragma once

#include "map/MapId.h"
#include "types/MapMemoryChunk.h"
#include "types/Vec2.h"

#include "json.hpp"
using json = ::nlohmann::json;

// Map memory struct.
struct MapMemory
{
  IntVec2 size;
  std::vector<MapMemoryChunk> chunks;
};

/// Template to use for making new components.
class ComponentSpacialMemory
{
public:

  friend void from_json(json const& j, ComponentSpacialMemory& obj);
  friend void to_json(json& j, ComponentSpacialMemory const& obj);

protected:

private:
  /// Spacial memory of each map.
  std::unordered_map<MapId, MapMemory> m_memory;
};

