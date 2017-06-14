#include "components/ComponentTemplate.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, Component& obj)
  {
    obj = Component();

    // *** add Component-specific assignments here ***
    //JSONUtils::doIfPresent(j, "member", [&](auto& value) { obj.m_member = value; });
  }

  void to_json(json& j, Component const& obj)
  {
    j = json::object();
    // *** add Component-specific assignments here ***
    //j["member"] = obj.m_member;
  }

} // end namespace