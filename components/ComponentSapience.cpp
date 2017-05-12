#include "components/ComponentSapience.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentSapience& obj)
{
  obj = ComponentSapience();

  // *** add Component-specific assignments here ***
  //JSONUtils::doIfPresent(j, "member", [&](auto& value) { obj.m_member = value; });
}

void to_json(json& j, ComponentSapience const& obj)
{
  j = json::object();
  // *** add Component-specific assignments here ***
  //j["member"] = obj.m_member;
}

ActionDeque& ComponentSapience::pendingActions()
{
  return m_pendingActions;
}

ActionDeque const& ComponentSapience::pendingActions() const
{
  return m_pendingActions;
}
