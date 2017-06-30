#pragma once

#include "types/common.h"
#include "types/EntitySpecs.h"

#include "json.hpp"
using json = ::nlohmann::json;

/// Represents the memory of a map square.

class MapMemoryChunk
{
public:
  MapMemoryChunk()
    :
    m_entitySpecs{},
    m_when{}
  {}

  MapMemoryChunk(std::vector<EntitySpecs> entitySpecs, ElapsedTicks when)
    :
    m_entitySpecs{ entitySpecs },
    m_when{ when }
  {}

  friend void from_json(json const& j, MapMemoryChunk& obj);
  friend void to_json(json& j, MapMemoryChunk const& obj);

  virtual ~MapMemoryChunk()
  {}

  std::vector<EntitySpecs> const& getSpecs() const
  {
    return m_entitySpecs;
  }

  ElapsedTicks getTimeOfMemory()
  {
    return m_when;
  }

private:
  /// Collection of entity specs remembered. Map tile floor and space should be first in the collection.
  std::vector<EntitySpecs> m_entitySpecs;

  /// Elapsed game time when the tile was remembered.
  ElapsedTicks m_when;
};