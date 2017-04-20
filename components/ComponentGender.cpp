#include "components/ComponentGender.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentGender& obj)
{
  if (j.is_object() && j.size() != 0)
  {
    // *** add Component-specific assignments here ***
    // JSONUtils::setIfPresent(obj.m_member, j, "member");
  }
  else
  {
    obj = ComponentGender();
  }
}

void to_json(json& j, ComponentGender const& obj)
{
  j = json::object();
  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}
