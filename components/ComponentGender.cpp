#include "components/ComponentGender.h"

#include "game/GameState.h"

void from_json(json const& j, ComponentGender& obj)
{
  if (!j.is_object() || j.size() == 0)
  {
    obj = Component();
  }
  else
  {
    // *** add Component-specific assignments here ***
    //obj.m_member = j["member"];
  }
}

void to_json(json& j, ComponentGender const& obj)
{
  j = json::object();
  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}
