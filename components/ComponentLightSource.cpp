#include "stdafx.h"

#include "components/ComponentLightSource.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

  void from_json(json const& j, ComponentLightSource& obj)
  {
    obj = ComponentLightSource();

    // *** add Component-specific assignments here ***
    JSONUtils::doIfPresent(j, "lit", [&](auto& value) { obj.m_lit = value; });
    JSONUtils::doIfPresent(j, "color", [&](auto& value) { obj.m_lightColor = value; });
    JSONUtils::doIfPresent(j, "strength", [&](auto& value) { obj.m_lightStrength = value; });
  }

  void to_json(json& j, ComponentLightSource const& obj)
  {
    j = json::object();
    // *** add Component-specific assignments here ***
    j["lit"] = obj.m_lit;
    j["color"] = obj.m_lightColor;
    j["strength"] = obj.m_lightStrength;
  }

  bool& ComponentLightSource::lit()
  {
    return m_lit;
  }

  bool const& ComponentLightSource::lit() const
  {
    return m_lit;
  }

  Color& ComponentLightSource::color()
  {
    return m_lightColor;
  }

  Color const& ComponentLightSource::color() const
  {
    return m_lightColor;
  }

  int& ComponentLightSource::strength()
  {
    return m_lightStrength;
  }

  int const& ComponentLightSource::strength() const
  {
    return m_lightStrength;
  }

} // end namespace