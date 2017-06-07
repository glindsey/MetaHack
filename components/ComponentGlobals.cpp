#include "components/ComponentGlobals.h"

#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{
  void from_json(json const& j, Components::ComponentGlobals& obj)
  {
    obj = Components::ComponentGlobals();
    JSONUtils::doIfPresent(j, "player", [&](auto& value) { obj.m_player = value; });
  }

  void to_json(json& j, Components::ComponentGlobals const& obj)
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

} // end namespace