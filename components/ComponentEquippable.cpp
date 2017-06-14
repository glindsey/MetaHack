#include "stdafx.h"

#include "components/ComponentEquippable.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentEquippable& obj)
  {
    obj = ComponentEquippable();

    /// @todo WRITE ME

    // *** add Component-specific assignments here ***
    //JSONUtils::doIfPresent(j, "member", [&](auto& value) { obj.m_member = value; });
  }

  void to_json(json& j, ComponentEquippable const& obj)
  {
    j = json::object();

    /// @todo WRITE ME

    // *** add Component-specific assignments here ***
    //j["member"] = obj.m_member;
  }

  std::set<BodyPart> const& ComponentEquippable::equippableOn() const
  {
    return m_equippableOn;
  }

} // end namespace