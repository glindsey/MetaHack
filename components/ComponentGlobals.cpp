#include "components/ComponentGlobals.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentGlobals& obj)
{
  obj = ComponentGlobals();
  JSONUtils::doIfPresent(j, "player", [&](auto& value) { obj.m_player = value; });
}

void to_json(json& j, ComponentGlobals const& obj)
{
  j = json::object();
  j["player"] = obj.m_player;
}

EntityId ComponentGlobals::player() const
{
  return m_player;
}

void ComponentGlobals::setPlayer(EntityId player)
{
  m_player = player;
}
