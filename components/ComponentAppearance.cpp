#include "components/ComponentAppearance.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentAppearance& obj)
{
  obj = ComponentAppearance();

  if (j.is_object() && j.size() != 0)
  {
    JSONUtils::setIfPresent(obj.m_opacity, j, "opacity");
  }
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