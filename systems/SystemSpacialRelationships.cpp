#include "stdafx.h"

#include "systems/SystemSpacialRelationships.h"

#include "components/ComponentGlobals.h"
#include "components/ComponentInventory.h"
#include "components/ComponentPosition.h"
#include "systems/SystemJanitor.h"
#include "utilities/MathUtils.h"

SystemSpacialRelationships::SystemSpacialRelationships(ComponentGlobals const& globals,
                                                       ComponentMap<ComponentInventory>& inventory, 
                                                       ComponentMap<ComponentPosition>& position) :
  SystemCRTP<SystemSpacialRelationships>({ EventEntityMoved::id, EventEntityChangedMaps::id }),
  m_globals{ globals },
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
  MapID oldMapID = position.map();
  EntityId oldLocation = position.parent();

  if (newLocation == oldLocation)
  {
    // We're already there!
    return true;
  }

  if (newInventory.canContain(entity))
  {
    if (newInventory.add(entity, (m_globals.player() == entity)) == true)
    {
      auto oldPosition = position; // copy for event broadcast

      // Try to lock our old location.
      if (oldLocation != EntityId::Mu())
      {
        m_inventory[oldLocation].remove(entity);
      }

      // Set the location to the new location.
      m_position[entity].set(newLocation);

      MapID newMapID = position.map();
      if (oldMapID != newMapID)
      {
        broadcast(EventEntityChangedMaps(entity));
      }
      else
      {
        broadcast(EventEntityMoved(entity, oldPosition));
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

void SystemSpacialRelationships::setMap_V(MapID newMap)
{}

bool SystemSpacialRelationships::onEvent(Event const& event)
{
  auto id = event.getId();

  if (id == SystemJanitor::EventEntityMarkedForDeletion::id)
  {
    auto& castEvent = static_cast<SystemJanitor::EventEntityMarkedForDeletion const&>(event);
    auto entity = castEvent.m_entity;
    auto old_location = m_position[entity].parent();

    if (m_inventory.existsFor(entity))
    {
      // Spill the contents of this Entity into the Entity's location.
      /// @todo Reimplement me
      //spill();
    }

    if (old_location != EntityId::Mu())
    {
      m_inventory[old_location].remove(entity);
    }
  }

  return false;
}
