#include "components/ComponentActivity.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentActivity& obj)
{
  obj = ComponentActivity();
  JSONUtils::doIfPresent(j, "pending-actions", [&](json const& value) { obj.m_pendingActions = value; });
}

void to_json(json& j, ComponentActivity const& obj)
{
  j = json::object();
  j["pending-actions"] = obj.m_pendingActions;
}

Actions::ActionQueue& ComponentActivity::pendingActions()
{
  return m_pendingActions;
}

Actions::ActionQueue const& ComponentActivity::pendingActions() const
{
  return m_pendingActions;
}
