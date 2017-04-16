#include "components/ComponentPosition.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"

void from_json(json const& j, ComponentPosition& obj)
{
  if (!j.is_object() || j.size() == 0)
  {
    obj = ComponentPosition();
  }
  else
  {
    obj.m_parent = j["parent"];
    obj.m_coords = j["coords"];
  }
}

void to_json(json& j, ComponentPosition const& obj)
{
  j = json::object();
  j["parent"] = obj.m_parent;
  j["coords"] = obj.m_coords;
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
    m_parent = id;
    m_map = MapId::Null();
    m_coords = { 0, 0 };
  }
}

void ComponentPosition::set(IntVec2 coords)
{
  if (m_coords != coords)
  {
    m_parent = EntityId::Mu();
    m_coords = coords;
  }
}

void ComponentPosition::set(MapId map, IntVec2 coords)
{
  if (m_map != map || m_coords != coords)
  {
    m_parent = EntityId::Mu();
    m_map = map;
    m_coords = coords;
  }
}

EntityId ComponentPosition::parent()
{
  return m_parent;
}

MapId ComponentPosition::map()
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
