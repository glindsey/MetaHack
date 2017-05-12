#include "components/ComponentSapience.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentSapience& obj)
{
  obj = ComponentSapience();
  JSONUtils::doIfPresent(j, "pending-actions", [&](json const& value) { obj.m_pendingActions = value; });
}

void to_json(json& j, ComponentSapience const& obj)
{
  j = json::object();
  j["pending-actions"] = obj.m_pendingActions;
}

Actions::ActionQueue& ComponentSapience::pendingActions()
{
  return m_pendingActions;
}

Actions::ActionQueue const& ComponentSapience::pendingActions() const
{
  return m_pendingActions;
}
