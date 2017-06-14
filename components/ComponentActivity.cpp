#include "stdafx.h"

#include "components/ComponentActivity.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{
  void from_json(json const& j, ComponentActivity& obj)
  {
    obj = ComponentActivity();
    JSONUtils::doIfPresent(j, "pending-actions", [&](json const& value) { obj.m_pendingActions = value; });
    JSONUtils::doIfPresent(j, "busy-ticks", [&](json const& value) { obj.m_busyTicks = value; });
  }

  void to_json(json& j, ComponentActivity const& obj)
  {
    j = json::object();
    j["pending-actions"] = obj.m_pendingActions;
    j["busy-ticks"] = obj.m_busyTicks;
  }

  Actions::ActionQueue& ComponentActivity::pendingActions()
  {
    return m_pendingActions;
  }

  Actions::ActionQueue const& ComponentActivity::pendingActions() const
  {
    return m_pendingActions;
  }

  bool ComponentActivity::actionPendingOrInProgress() const
  {
    return (!pendingActions().empty() || busyTicks() > 0);
  }

  int ComponentActivity::busyTicks() const
  {
    return m_busyTicks;
  }

  void ComponentActivity::clearBusyTicks()
  {
    m_busyTicks = 0;
  }

  void ComponentActivity::setBusyTicks(int value)
  {
    m_busyTicks = value;
    if (m_busyTicks < 0) m_busyTicks = 0;
  }

  void ComponentActivity::incBusyTicks(int value)
  {
    m_busyTicks += value;
    if (m_busyTicks < 0) m_busyTicks = 0;
  }

  void ComponentActivity::decBusyTicks(int value)
  {
    incBusyTicks(-value);
  }

} // end namespace Components
