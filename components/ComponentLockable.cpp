#include "stdafx.h"

#include "components/ComponentLockable.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentLockable& obj)
  {
    obj = ComponentLockable();
    JSONUtils::doIfPresent(j, "locked", [&](auto& value) { obj.m_locked = value; });
  }

  void to_json(json& j, ComponentLockable const& obj)
  {
    j = json::object();
    j["locked"] = obj.m_locked;
  }

  bool ComponentLockable::isLocked() const
  {
    return m_locked;
  }

  void ComponentLockable::setLocked(bool value)
  {
    m_locked = value;
  }

} // end namespace