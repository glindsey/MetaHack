#include "components/ComponentPosition.h"

#include "components/ComponentsManager.h"
#include "game/GameState.h"

void from_json(json const& j, ComponentPosition& obj)
{
  obj.m_parent = j["parent"];
  obj.m_coords = j["coords"];
}

void to_json(json& j, ComponentPosition const& obj)
{
  j["parent"] = obj.m_parent;
  j["coords"] = obj.m_coords;
}

void ComponentPosition::set(EntityId id)
{
  m_parent = id;
  m_coords = { 0, 0 };
}

void ComponentPosition::set(IntVec2 coords)
{
  m_parent = EntityId::Mu();
  m_coords = coords;
}

EntityId ComponentPosition::parent()
{
  return m_parent;
}

IntVec2 ComponentPosition::coords()
{
  if (m_parent != EntityId::Mu())
  {
    return GAME.components().position[m_parent].coords();
  }
  else
  {
    return m_coords;
  }
}
