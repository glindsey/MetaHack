#include "components/ComponentTemplate.h"

#include "game/GameState.h"

void from_json(json const& j, Component& obj)
{
  obj = Component();

  if (j.is_object() && j.size() != 0)
  {
    // *** add Component-specific assignments here ***
    //obj.m_member = j["member"];
  }
}

void to_json(json& j, Component const& obj)
{
  j = json::object();
  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}
