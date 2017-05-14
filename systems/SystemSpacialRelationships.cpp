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

void SystemSpacialRelationships::doCycleUpdate()
{}

bool SystemSpacialRelationships::moveEntityInto(EntityId entity, EntityId newLocation)
{
  // If Entity doesn't have a Position component, bail.
  if (!m_position.existsFor(entity)) return false;

  // If new location doesn't have an Inventory component, bail.
  if (!m_inventory.existsFor(newLocation)) return false;

  auto& position = m_position[entity];
  auto& newInventory = m_inventory[newLocation];
  MapId oldMapId = position.map();
  EntityId oldLocation = position.parent();

  if (newLocation == oldLocation)
  {
    // We're already there!
    return true;
  }

  if (newInventory.canContain(entity))
  {
    if (newInventory.add(entity) == true)
    {
      // Try to lock our old location.
      if (oldLocation != EntityId::Mu())
      {
        m_inventory[oldLocation].remove(entity);
      }

      // Set the location to the new location.
      m_position[entity].set(newLocation);

      MapId newMapId = position.map();
      if (oldMapId != newMapId)
      {
        broadcast(EventEntityChangedMaps(entity));
      }
      else
      {
        broadcast(EventEntityMoved(entity));
      }
      return true;
    } // end if (add to new inventory was successful)
  } // end if (canContain is true)

  return false;
}

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

std::unordered_set<EventID> SystemSpacialRelationships::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  events.insert(EventEntityMoved::id());
  events.insert(EventEntityChangedMaps::id());
  return events;
}

bool SystemSpacialRelationships::onEvent_NVI(Event const& event)
{
  return false;
}
