#pragma once

#include <vector>

#include "types/MapMemoryChunk.h"
#include "types/Vec2.h"

#include "json.hpp"
using json = ::nlohmann::json;

// Map memory struct.
class MapMemory
{
public:

  friend void from_json(json const& j, MapMemory& obj);
  friend void to_json(json& j, MapMemory const& obj);

  IntVec2 const& size() const;

  MapMemoryChunk& at(IntVec2 coords);
  MapMemoryChunk const& at(IntVec2 coords) const;
  void clear();
  bool contains(IntVec2 coords) const;
  MapMemoryChunk& operator[](IntVec2 coords);
  void resize(IntVec2 size);
  MapMemoryChunk const& valueOr(IntVec2 coords, MapMemoryChunk const& defaultValue) const;

protected:
  unsigned int index(IntVec2 coords) const;

private:
  IntVec2 m_size;
  std::map<unsigned int, MapMemoryChunk> m_chunks;
};