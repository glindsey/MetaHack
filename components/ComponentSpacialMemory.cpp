#include "components/ComponentSpacialMemory.h"

#include "game/GameState.h"

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
