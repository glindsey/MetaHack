#include "components/ComponentDigestiveSystem.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentDigestiveSystem& obj)
{
  obj = ComponentDigestiveSystem();
  JSONUtils::doIfPresent(j, "allowed-foods", [&](auto& value) { obj.m_allowedFoods = value; });
  JSONUtils::doIfPresent(j, "preferred-foods", [&](auto& value) { obj.m_preferredFoods = value; });
  JSONUtils::doIfPresent(j, "taboo-foods", [&](auto& value) { obj.m_tabooFoods = value; });
  JSONUtils::doIfPresent(j, "needs-mouth", [&](auto& value) { obj.m_needsMouth = value; });
}

void to_json(json& j, ComponentDigestiveSystem const& obj)
{
  j = json::object();
  j["allowed-foods"] = obj.m_allowedFoods;
  j["preferred-foods"] = obj.m_preferredFoods;
  j["taboo-foods"] = obj.m_tabooFoods;
  j["needs-mouth"] = obj.m_needsMouth;
}

bool ComponentDigestiveSystem::needsMouth() const
{
  return m_needsMouth;
}
