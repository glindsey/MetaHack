#include "stdafx.h"

#include "map/MapMemory.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, MapMemory& obj)
{
  obj.m_size = IntVec2(0, 0);
  obj.m_chunks.clear();

  if (j.is_object() && j.size() != 0)
  {
    JSONUtils::setIfPresent(obj.m_size, j, "size");

    if (j.count("chunks") != 0)
    {
      for (auto citer = j["chunks"].cbegin(); citer != j["chunks"].cend(); ++citer)
      {
        obj.m_chunks[std::stoul(citer.key())] = citer.value();
      }
    }
  }
}

void to_json(json& j, MapMemory const& obj)
{
  j = json::object();
  j["size"] = obj.m_size;
  j["chunks"] = json::object();
  for (auto citer = obj.m_chunks.cbegin(); citer != obj.m_chunks.cend(); ++citer)
  {
    j["chunks"][std::to_string(citer->first)] = citer->second;
  }
}

IntVec2 const& MapMemory::size() const
{
  return m_size;
}

MapMemoryChunk& MapMemory::at(IntVec2 coords)
{
  return m_chunks.at(index(coords));
}

MapMemoryChunk const& MapMemory::at(IntVec2 coords) const
{
  return m_chunks.at(index(coords));
}

void MapMemory::clear()
{
  m_chunks.clear();
}

bool MapMemory::contains(IntVec2 coords) const
{
  return m_chunks.count(index(coords)) != 0;
}

MapMemoryChunk& MapMemory::operator[](IntVec2 coords)
{
  if (!contains(coords))
  {
    m_chunks[index(coords)] = MapMemoryChunk();
  }
  return m_chunks.at(index(coords));
}

void MapMemory::resize(IntVec2 size)
{
  Assert("Map", (size.x > 0 && size.y > 0), "Invalid size (" << size.x << ", " << size.y << ") specified");
  clear();
  m_size = size;
}

MapMemoryChunk const& MapMemory::valueOr(IntVec2 coords, MapMemoryChunk const& defaultValue) const
{
  if (!contains(coords))
  {
    return defaultValue;
  }
  return m_chunks.at(index(coords));
}

unsigned int MapMemory::index(IntVec2 coords) const
{
  return (m_size.x * coords.y) + coords.x;
}
