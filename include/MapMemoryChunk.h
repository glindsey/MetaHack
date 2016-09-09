#pragma once

#include "stdafx.h"

/// Represents the memory of a map square.

class MapMemoryChunk
{
public:
  MapMemoryChunk()
    :
    m_type{},
    m_tick{}
  {}

  MapMemoryChunk(std::string type, GameTick tick)
    :
    m_type{ type },
    m_tick{ tick }
  {}

  virtual ~MapMemoryChunk()
  {}

  std::string const& get_type()
  {
    return m_type;
  }

  GameTick get_tick()
  {
    return m_tick;
  }

private:
  /// std::string representing the type of tile remembered.
  std::string m_type;

  /// Tick of the game clock when the tile was remembered.
  GameTick m_tick;
};