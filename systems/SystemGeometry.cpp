#include "stdafx.h"

#include "components/ComponentGlobals.h"
#include "components/ComponentInventory.h"
#include "components/ComponentPosition.h"
#include "services/Service.h"
#include "services/IMessageLog.h"
#include "systems/SystemGeometry.h"
#include "systems/SystemJanitor.h"
#include "systems/SystemNarrator.h"
#include "utilities/MathUtils.h"
#include "utilities/Shortcuts.h"

namespace Systems
{

  Geometry::Geometry(Systems::Janitor& janitor,
                     Systems::Narrator& narrator,
                     Components::ComponentGlobals const& globals,
                     Components::ComponentMap<Components::ComponentInventory>& inventory,
                     Components::ComponentMap<Components::ComponentPosition>& position) :
    CRTP<Geometry>({ EventEntityMoved::id, EventEntityChangedMaps::id }),
    m_janitor{ janitor },
    m_narrator{ narrator },
    m_globals{ globals },
    m_inventory{ inventory },
    m_position{ position }
  {}

  Geometry::~Geometry()
  {}

  void Geometry::doCycleUpdate()
  {}

  bool Geometry::moveEntityInto(EntityId entity, EntityId newLocation)
  {
    Assert("Geometry", m_position.existsFor(entity),
           "Asked to move entity " << entity << " into location " << newLocation << ", but the entity doesn't have a Position component!");

    Assert("Geometry", m_inventory.existsFor(newLocation),
           "Asked to move entity " << entity << " into location " << newLocation << ", but the location doesn't have an Inventory component!");

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
        if (oldLocation != EntityId::Void)
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

  /// @todo Add collision events for Mechanics system to process.
  void Geometry::spill(EntityId entity)
  {
    // If entity doesn't have an inventory or a position, bail.
    if (!m_inventory.existsFor(entity) || !m_position.existsFor(entity)) return;

    auto& entityInventory = m_inventory.of(entity);
    auto& entityPosition = m_position.of(entity);
    auto entityParent = entityPosition.parent();
    std::string message;
    bool success = false;
  
    // Create a copy of this entity's inventory, since we'll be modifying it
    // as we go along.
    auto tempInventoryCopy = entityInventory;
    auto parent = entityPosition.parent();

    // Step through all contents of this Entity.
    for (auto itemPair : tempInventoryCopy)
    {
      EntityId item = itemPair.second;
      if (m_inventory.existsFor(entityParent))
      {
        auto& parentInventory = m_inventory.of(entityParent);
        if (parentInventory.canContain(item))
        {
          json arguments;
          arguments["subject"] = m_narrator.getDescriptiveString(item);
          arguments["the_location"] = m_narrator.getDescriptiveString(entity);

          // Try to move this into the Entity's location.
          success = moveEntityInto(item, entityParent);

          if (success)
          {
            putMsg(m_narrator.makeTr("YOU_TUMBLE_OUT_OF_THE_LOCATION", arguments));
          }
          else
          {
            putMsg(m_narrator.makeTr("YOU_VANISH_IN_A_PUFF_OF_LOGIC", arguments));
            m_janitor.markForDeletion(item);
          }
          //notifyObservers(Event::Updated);
  
        } // end if (canContain)
      } // end if (container location is not Void)
      else
      {
        // Container's location is Void, so just destroy it without a message.
        m_janitor.markForDeletion(item);
      }
    } // end for (contents of Entity)
  }

  bool Geometry::firstCanReachSecond(EntityId first, EntityId second) const
  {
    return (areAdjacent(first, second) ||
            firstIsDirectlyInsideSecond(first, second));
  }

  bool Geometry::insideAnotherEntity(EntityId entity) const
  {
    // If entity is missing a position component, bail.
    if (!m_position.existsFor(entity)) return false;
    auto& position = m_position.of(entity);
    auto& parent = position.parent();

    if (parent == EntityId::Void)
    {
      // Entity is a part of the MapTile such as the floor.
      return false;
    }

    auto& grandparent = m_position.of(parent).parent();
    if (grandparent == EntityId::Void)
    {
      // Entity is directly on the floor.
      return false;
    }
    return true;
  }

  bool Geometry::firstIsDirectlyInsideSecond(EntityId first, EntityId second) const
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
    if (firstParent != EntityId::Void)
    {
      // If the other entity is our parent, return true.
      if (firstParent == second) return true;
    }

    // Otherwise return false.
    return false;
  }

  bool Geometry::firstIsInsideSecond(EntityId first, EntityId second) const
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
    if (firstParent != EntityId::Void)
    {
      // If the other entity is our parent, return true.
      if (firstParent == second) return true;

      // Otherwise, return whether our parent is inside the other entity.
      return m_position.of(firstParent).isInside(second);
    }

    // If we have no parent, return false.
    return false;
  }

  bool Geometry::areAdjacent(EntityId first, EntityId second) const
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

    return Math::adjacent(firstPosition.coords(), secondPosition.coords());
  }

  void Geometry::setMap_V(MapID newMap)
  {}

  bool Geometry::onEvent(Event const& event)
  {
    auto id = event.getId();

    if (id == Janitor::EventEntityMarkedForDeletion::id)
    {
      auto& castEvent = static_cast<Janitor::EventEntityMarkedForDeletion const&>(event);
      auto entity = castEvent.m_entity;
      auto old_location = m_position[entity].parent();

      if (m_inventory.existsFor(entity))
      {
        // Spill the contents of this Entity into the Entity's location.
        /// @todo Reimplement me
        //spill();
      }

      if (old_location != EntityId::Void)
      {
        m_inventory[old_location].remove(entity);
      }
    }

    return false;
  }

} // end namespace Systems
