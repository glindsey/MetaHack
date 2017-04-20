#include "components/ComponentSpacialMemory.h"

#include "game/GameState.h"

/// @todo Regarding memory, it would be AWESOME if it could fade out
///       VERY gradually, over a long period of time. Seeing it would
///       reset the memory counter to 0, or possibly just add a large
///       amount to the counter so that places you see more frequently
///       stay in your mind longer.

void from_json(json const& j, ComponentSpacialMemory& obj)
{
  obj.m_memory.clear();

  if (j.is_object() && j.size() != 0)
  {
    for (auto citer = j.cbegin(); citer != j.cend(); ++citer)
    {
      obj.m_memory[citer.key()] = citer.value();
    }
  }
}

void to_json(json& j, ComponentSpacialMemory const& obj)
{
  j = json::object();

  for (auto citer = obj.m_memory.cbegin(); citer != obj.m_memory.cend(); ++citer)
  {
    j[static_cast<std::string>(citer->first)] = citer->second;
  }
}

bool ComponentSpacialMemory::containsMap(MapId map)
{
  return (m_memory.count(map) != 0);
}

MapMemory& ComponentSpacialMemory::ofMap(MapId map)
{
  return m_memory.at(map);
}

MapMemory const& ComponentSpacialMemory::ofMap(MapId map) const
{
  return m_memory.at(map);
}

MapMemory & ComponentSpacialMemory::operator[](MapId map)
{
  return m_memory[map];
}