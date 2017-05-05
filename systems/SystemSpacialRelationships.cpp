#include "stdafx.h"

#include "systems/SystemSpacialRelationships.h"

#include "components/ComponentInventory.h"
#include "components/ComponentPosition.h"
#include "utilities/MathUtils.h"

SystemSpacialRelationships::SystemSpacialRelationships(ComponentMap<ComponentInventory>& inventory, 
                                                       ComponentMap<ComponentPosition>& position) :
  SystemCRTP<SystemSpacialRelationships>(),
  m_inventory{ inventory },
  m_position{ position }
{}

SystemSpacialRelationships::~SystemSpacialRelationships()
{}

void SystemSpacialRelationships::recalculate()
{}

bool SystemSpacialRelationships::firstCanReachSecond(EntityId first, EntityId second) const
{
  return (areAdjacent(first, second) || 
          firstIsDirectlyInsideSecond(first, second));
}

bool SystemSpacialRelationships::insideAnotherEntity(EntityId entity) const
{
  // If entity is missing a position component, bail.
  if (!m_position.existsFor(entity)) return false;
  auto& position = m_position.of(entity);
  auto& parent = position.parent();

  if (parent == EntityId::Mu())
  {
    // Entity is a part of the MapTile such as the floor.
    return false;
  }

  auto& grandparent = m_position.of(parent).parent();
  if (grandparent == EntityId::Mu())
  {
    // Entity is directly on the floor.
    return false;
  }
  return true;
}

bool SystemSpacialRelationships::firstIsDirectlyInsideSecond(EntityId first, EntityId second) const
{
  // If either entity doesn't have a position component, bail.
  if (!m_position.existsFor(first) || !m_position.existsFor(second))
  {
    return false;
  }

  auto& firstPosition = m_position.of(first);
  auto& secondPosition = m_position.of(second);

  // If first has a parent...
  auto& firstParent = firstPosition.parent();
  if (firstParent != EntityId::Mu())
  {
    // If the other entity is our parent, return true.
    if (firstParent == second) return true;
  }

  // Otherwise return false.
  return false;
}

bool SystemSpacialRelationships::firstIsInsideSecond(EntityId first, EntityId second) const
{
  // If either entity doesn't have a position component, bail.
  if (!m_position.existsFor(first) || !m_position.existsFor(second))
  {
    return false;
  }

  auto& firstPosition = m_position.of(first);
  auto& secondPosition = m_position.of(second);

  // If first has a parent...
  auto& firstParent = firstPosition.parent();
  if (firstParent != EntityId::Mu())
  {
    // If the other entity is our parent, return true.
    if (firstParent == second) return true;

    // Otherwise, return whether our parent is inside the other entity.
    return m_position.of(firstParent).isInside(second);
  }

  // If we have no parent, return false.
  return false;
}

bool SystemSpacialRelationships::areAdjacent(EntityId first, EntityId second) const
{
  // If either entity doesn't have a position component, bail.
  if (!m_position.existsFor(first) || !m_position.existsFor(second))
  {
    return false;
  }

  auto& firstPosition = m_position.of(first);
  auto& secondPosition = m_position.of(second);

  // If the two are not on the same map, bail.
  if (firstPosition.map() != secondPosition.map())
  {
    return false;
  }

  return adjacent(firstPosition.coords(), secondPosition.coords());
}

void SystemSpacialRelationships::setMapNVO(MapId newMap)
{}
