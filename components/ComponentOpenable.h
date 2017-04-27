#pragma once

#include "json.hpp"
using json = ::nlohmann::json;

/// Component that indicates an entity can be opened/closed.
class ComponentOpenable
{
public:

  friend void from_json(json const& j, ComponentOpenable& obj);
  friend void to_json(json& j, ComponentOpenable const& obj);

  bool isOpen() const;
  void setOpen(bool value);

protected:

private:
  /// Boolean indicating whether this entity is open.
  bool m_open = true;

};
