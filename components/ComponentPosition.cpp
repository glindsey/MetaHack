#include "components/ComponentPosition.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "utilities/JSONUtils.h"

void from_json(json const& j, ComponentPosition& obj)
{
  obj = ComponentPosition();
  JSONUtils::doIfPresent(j, "coords", [&](auto& value) { obj.m_coords = value; });
  JSONUtils::doIfPresent(j, "map", [&](auto& value) { obj.m_map = value; });
  JSONUtils::doIfPresent(j, "parent", [&](auto& value) { obj.m_parent = value; });
}

void to_json(json& j, ComponentPosition const& obj)
{
  j = json::object();
  j["coords"] = obj.m_coords;
  j["map"] = obj.m_map;
  j["parent"] = obj.m_parent;
}

ComponentPosition::ComponentPosition()
{}

ComponentPosition::ComponentPosition(EntityId id)
  : m_parent{ id }
{}

ComponentPosition::ComponentPosition(MapId map, IntVec2 coords)
  : m_map{ map }, m_coords{ coords }
{}

ComponentPosition::~ComponentPosition()
{}

void ComponentPosition::set(EntityId id)
{
  if (m_parent != id)
  {
    m_coords = { 0, 0 };
    m_map = MapId::Null();
    m_parent = id;
  }
}

void ComponentPosition::set(IntVec2 coords)
{
  if (m_coords != coords)
  {
    m_coords = coords;
    m_parent = EntityId::Mu();
  }
}

void ComponentPosition::set(MapId map, IntVec2 coords)
{
  if (m_map != map || m_coords != coords)
  {
    m_coords = coords;
    m_map = map;
    m_parent = EntityId::Mu();
  }
}

EntityId ComponentPosition::parent() const
{
  return m_parent;
}

MapId ComponentPosition::map() const
{
  if (m_parent != EntityId::Mu())
  {
    return GAME.components().position[m_parent].map();
  }
  else
  {
    return m_map;
  }
}

IntVec2 ComponentPosition::coords() const
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
