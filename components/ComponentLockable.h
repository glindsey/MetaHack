#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that indicates an entity can be locked/unlocked.
class ComponentLockable
{
public:

  friend void from_json(json const& j, ComponentLockable& obj);
  friend void to_json(json& j, ComponentLockable const& obj);

  bool isLocked() const;
  void setLocked(bool value);

protected:

private:
  /// Boolean indicating whether component is locked.
  bool m_locked = false;

};
