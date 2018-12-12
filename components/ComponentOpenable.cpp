#include "components/ComponentOpenable.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentOpenable& obj)
  {
    obj = ComponentOpenable();
    JSONUtils::doIfPresent(j, "open", [&](auto& value) { obj.m_open = value; });
  }

  void to_json(json& j, ComponentOpenable const& obj)
  {
    j = json::object();
    j["open"] = obj.m_open;
  }

  bool ComponentOpenable::isOpen() const
  {
    return m_open;
  }

  void ComponentOpenable::setOpen(bool value)
  {
    m_open = value;
  }

} // end namespace
