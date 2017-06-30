#include "stdafx.h"

#include "components/ComponentPosition.h"

#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "utilities/JSONUtils.h"

namespace Components
{

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
    : m_map{ "" }, m_coords{ 0, 0 }, m_parent{ EntityId::Void }
  {}

  ComponentPosition::ComponentPosition(EntityId id)
    : m_map{ "" }, m_coords{ 0, 0 }, m_parent{ id }
  {}

  ComponentPosition::ComponentPosition(MapID map, IntVec2 coords)
    : m_map{ map }, m_coords{ coords }, m_parent{ EntityId::Void }
  {}

  ComponentPosition::~ComponentPosition()
  {}

  void ComponentPosition::set(EntityId id)
  {
    if (m_parent != id)
    {
      m_coords = { 0, 0 };
      m_map = "";
      m_parent = id;
    }
  }

  void ComponentPosition::set(IntVec2 coords)
  {
    if (m_coords != coords)
    {
      m_coords = coords;
      m_parent = EntityId::Void;
    }
  }

  void ComponentPosition::set(MapID map, IntVec2 coords)
  {
    if (m_map != map || m_coords != coords)
    {
      m_coords = coords;
      m_map = map;
      m_parent = EntityId::Void;
    }
  }

  EntityId ComponentPosition::parent() const
  {
    return m_parent;
  }

  MapID ComponentPosition::map() const
  {
    if (m_parent != EntityId::Void)
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
    if (m_parent != EntityId::Void)
    {
      return GAME.components().position[m_parent].coords();
    }
    else
    {
      return m_coords;
    }
  }

  bool ComponentPosition::isInsideAnotherEntity() const
  {
    if (m_parent == EntityId::Void)
    {
      // Entity is a part of the MapTile such as the floor.
      return false;
    }

    auto& grandparent = COMPONENTS.position[m_parent].parent();
    if (grandparent == EntityId::Void)
    {
      // Entity is directly on the floor.
      return false;
    }
    return true;
  }

  bool ComponentPosition::isInside(EntityId id) const
  {
    // If other entity doesn't have a position component, bail.
    if (!COMPONENTS.position.existsFor(id))
    {
      return false;
    }

    auto otherPosition = COMPONENTS.position[id];

    // If we have a parent...
    if (m_parent != EntityId::Void)
    {
      // If the other entity is our parent, return true.
      if (m_parent == id) return true;

      // Otherwise, return whether our parent is inside the other entity.
      return COMPONENTS.position[m_parent].isInside(id);
    }

    // If we have no parent, return false.
    return false;
  }

  bool ComponentPosition::isAdjacentTo(EntityId id) const
  {
    // If other entity doesn't have a position component, bail.
    if (!COMPONENTS.position.existsFor(id))
    {
      return false;
    }

    auto otherPosition = COMPONENTS.position[id];

    // If the two are not on the same map, bail.
    if (m_map != otherPosition.m_map)
    {
      return false;
    }

    return Math::adjacent(m_coords, otherPosition.m_coords);
  }

} // end namespace
