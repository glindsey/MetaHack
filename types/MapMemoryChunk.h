#pragma once

#include "stdafx.h"

/// Represents the memory of a map square.

class MapMemoryChunk
{
public:
  MapMemoryChunk()
    :
    m_type{},
    m_memory_time{}
  {}

  MapMemoryChunk(std::string type, ElapsedTime memory_type)
    :
    m_type{ type },
    m_memory_time{ memory_type }
  {}

  virtual ~MapMemoryChunk()
  {}

  std::string const& getType() const
  {
    return m_type;
  }

  ElapsedTime get_memory_time()
  {
    return m_memory_time;
  }

private:
  /// std::string representing the type of tile remembered.
  std::string m_type;

  /// Elapsed game time when the tile was remembered.
  ElapsedTime m_memory_time;
};