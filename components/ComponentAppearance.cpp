#include "components/ComponentAppearance.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{
  void from_json(json const& j, ComponentAppearance& obj)
  {
    obj = ComponentAppearance();
    JSONUtils::doIfPresent(j, "opacity", [&](auto& value) { obj.m_opacity = value; });
  }

  void to_json(json& j, ComponentAppearance const& obj)
  {
    j = json::object();
    j["opacity"] = obj.m_opacity;
  }

  ComponentAppearance::ComponentAppearance()
    : m_opacity{ Color::White }
  {}

  ComponentAppearance::ComponentAppearance(Color opacity)
    : m_opacity{ opacity }
  {}

  ComponentAppearance::~ComponentAppearance()
  {}

  Color& ComponentAppearance::opacity()
  {
    return m_opacity;
  }

  Color const& ComponentAppearance::opacity() const
  {
    return m_opacity;
  }

  bool ComponentAppearance::isTotallyOpaque() const
  {
    return (m_opacity == Color::White);
  }

  bool ComponentAppearance::isTotallyTransparent() const
  {
    return (m_opacity == Color::Black);
  }

} // end namespace Components
