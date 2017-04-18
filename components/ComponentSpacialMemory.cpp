#include "components/ComponentSpacialMemory.h"

#include "game/GameState.h"

void from_json(json const& j, ComponentSpacialMemory& obj)
{
  obj.m_memory.clear();

  if (j.is_object() && j.size() != 0)
  {
    // *** add ComponentSpacialMemory-specific assignments here ***
    //obj.m_member = j["member"];
  }
}

void to_json(json& j, ComponentSpacialMemory const& obj)
{
  j = json::object();
  // *** add ComponentSpacialMemory-specific assignments here ***
  //j["member"] = obj.m_member;
}
