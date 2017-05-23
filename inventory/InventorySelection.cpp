#include "stdafx.h"

#include "inventory/InventorySelection.h"

#include "AssertHelper.h"
#include "components/ComponentInventory.h"
#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "game/App.h"
#include "game/GameState.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

InventorySelection::InventorySelection()
  :
  Object({
  EventEntityChanged::id,
  EventSelectionChanged::id })
{}

InventorySelection::InventorySelection(EntityId entity)
  :
  InventorySelection()
{
  setViewed(entity);
}

InventorySelection::~InventorySelection()
{
  //dtor
}

EntityId InventorySelection::getViewed() const
{
  return m_viewed;
}

void InventorySelection::setViewed(EntityId entity)
{
  m_viewed = entity;
  m_selectedSlots.clear();

  broadcast(EventEntityChanged(entity));
}

void InventorySelection::toggleSelection(InventorySlot selection)
{
  if (m_viewed == EntityId::Mu())
  {
    return;
  }

  ComponentInventory& inventory = COMPONENTS.inventory[m_viewed];

  if (inventory.contains(selection))
  {
    auto iter = std::find(std::begin(m_selectedSlots),
                          std::end(m_selectedSlots),
                          selection);
    if (iter == std::end(m_selectedSlots))
    {
      CLOG(TRACE, "InventorySelection") <<
        "Adding slot " << static_cast<unsigned int>(selection) <<
        "to selected entities";
      m_selectedSlots.push_back(selection);
    }
    else
    {
      CLOG(TRACE, "InventorySelection") <<
        "Removing slot " << static_cast<unsigned int>(selection) <<
        "from selected entities";
      m_selectedSlots.erase(iter);
    }

    resetSelectedQuantity(); // will notify observers
  }
}

size_t InventorySelection::getSelectedSlotCount() const
{
  return m_selectedSlots.size();
}

std::vector<InventorySlot> const& InventorySelection::getSelectedSlots()
{
  return m_selectedSlots;
}

std::vector<EntityId> InventorySelection::getSelectedThings()
{
  std::vector<EntityId> entities;

  if (m_viewed != EntityId::Mu())
  {
    ComponentInventory& inventory = COMPONENTS.inventory[m_viewed];

    for (auto iter = std::begin(m_selectedSlots);
         iter != std::end(m_selectedSlots);
         ++iter)
    {
      EntityId entity = inventory[*iter];
      entities.push_back(entity);
    }
  }

  return entities;
}

void InventorySelection::clearSelectedSlots()
{
  m_selectedSlots.clear();
  broadcast(EventSelectionChanged(m_selectedSlots, m_selectedQuantity));
}

unsigned int InventorySelection::getSelectedQuantity() const
{
  return m_selectedQuantity;
}

unsigned int InventorySelection::getMaxQuantity() const
{
  unsigned int result;

  if (m_viewed == EntityId::Mu())
  {
    return 0;
  }

  ComponentInventory& inventory = COMPONENTS.inventory[m_viewed];

  if (m_selectedSlots.size() == 0)
  {
    result = 0;
  }
  else
  {
    EntityId entity = inventory[m_selectedSlots[0]];

    if (entity == EntityId::Mu())
    {
      result = 0;
    }
    else
    {
      result = COMPONENTS.quantity.valueOr(entity, 1);
    }
  }

  return result;
}

unsigned int InventorySelection::resetSelectedQuantity()
{
  m_selectedQuantity = getMaxQuantity();
  broadcast(EventSelectionChanged(m_selectedSlots, m_selectedQuantity));
  return m_selectedQuantity;
}

bool InventorySelection::setSelectedQuantity(unsigned int amount)
{
  if (amount > 0)
  {
    unsigned int maximum = getMaxQuantity();
    if (amount <= maximum)
    {
      m_selectedQuantity = amount;
      broadcast(EventSelectionChanged(m_selectedSlots, m_selectedQuantity));
      return true;
    }
  }
  return false;
}

bool InventorySelection::incSelectedQuantity()
{
  unsigned int maximum = getMaxQuantity();
  if (m_selectedQuantity < maximum)
  {
    ++(m_selectedQuantity);
    broadcast(EventSelectionChanged(m_selectedSlots, m_selectedQuantity));
    return true;
  }
  return false;
}

bool InventorySelection::decSelectedQuantity()
{
  if (m_selectedQuantity > 1)
  {
    --(m_selectedQuantity);
    broadcast(EventSelectionChanged(m_selectedSlots, m_selectedQuantity));
    return true;
  }
  return false;
}

EntityId InventorySelection::getEntity(InventorySlot selection)
{
  EntityId viewed = m_viewed;

  if (viewed == EntityId::Mu())
  {
    return EntityId::Mu();
  }

  ComponentInventory& inventory = COMPONENTS.inventory[m_viewed];

  if (inventory.contains(selection))
  {
    return inventory[selection];
  }
  else
  {
    CLOG(WARNING, "InventorySelection") <<
      "Requested non-existent inventory slot " <<
      static_cast<unsigned int>(selection) <<
      ", returning Mu!";
    return EntityId::Mu();
  }
}

char InventorySelection::getCharacter(InventorySlot slot)
{
  return getCharacter(static_cast<unsigned int>(slot));
}

char InventorySelection::getCharacter(unsigned int slot_number)
{
  char character;

  if (slot_number == 0)
  {
    character = '@';
  }
  else if (slot_number < 26)
  {
    character = '`' + static_cast<char>(slot_number);
  }
  else if (slot_number < 52)
  {
    character = '@' + static_cast<char>(slot_number - 26);
  }
  else
  {
    character = ' ';
  }

  return character;
}

InventorySlot InventorySelection::getSlot(char character)
{
  unsigned int slot_number;
  unsigned int char_number = static_cast<unsigned int>(character);

  Assert("Inventory",
         ((char_number >= 0x40) && (char_number <= 0x5a)) ||
         ((char_number >= 0x61) && (char_number <= 0x7a)),
         "tried to get the slot number for invalid character \"" << character << "\"");

  if (char_number == 0x40)
  {
    slot_number = 0;
  }
  else if (char_number <= 0x5a)
  {
    slot_number = char_number - 64;
  }
  else
  {
    slot_number = char_number - 70;
  }

  return static_cast<InventorySlot>(slot_number);
}
