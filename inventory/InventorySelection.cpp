#include "stdafx.h"

#include "inventory/InventorySelection.h"

#include "AssertHelper.h"
#include "components/ComponentManager.h"
#include "components/ComponentPhysical.h"
#include "game/App.h"
#include "game/GameState.h"
#include "inventory/Inventory.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

struct InventorySelection::Impl
{
  /// Entity whose contents (or surroundings) are currently being viewed.
  EntityId viewed;

  /// Vector of selected inventory slots.
  std::vector< InventorySlot > selected_slots;

  /// Reference to quantity of topmost selected item.
  unsigned int selected_quantity;


};

InventorySelection::InventorySelection()
  :
  pImpl(NEW Impl())
{
}

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

EntityId InventorySelection::get_viewed() const
{
  return pImpl->viewed;
}

void InventorySelection::setViewed(EntityId entity)
{
  pImpl->viewed = entity;
  pImpl->selected_slots.clear();

  broadcast(EventEntityChanged(entity));
}

void InventorySelection::toggle_selection(InventorySlot selection)
{
  if (pImpl->viewed == EntityId::Mu())
  {
    return;
  }

  Inventory& inventory = pImpl->viewed->getInventory();

  if (inventory.contains(selection))
  {
    auto iter = std::find(std::begin(pImpl->selected_slots),
                          std::end(pImpl->selected_slots),
                          selection);
    if (iter == std::end(pImpl->selected_slots))
    {
      CLOG(TRACE, "InventorySelection") <<
        "Adding slot " << static_cast<unsigned int>(selection) <<
        "to selected entities";
      pImpl->selected_slots.push_back(selection);
    }
    else
    {
      CLOG(TRACE, "InventorySelection") <<
        "Removing slot " << static_cast<unsigned int>(selection) <<
        "from selected entities";
      pImpl->selected_slots.erase(iter);
    }

    reset_selected_quantity(); // will notify observers
  }
}

size_t InventorySelection::get_selected_slot_count() const
{
  return pImpl->selected_slots.size();
}

std::vector<InventorySlot> const& InventorySelection::get_selected_slots()
{
  return pImpl->selected_slots;
}

std::vector<EntityId> InventorySelection::get_selected_things()
{
  std::vector<EntityId> entities;

  if (pImpl->viewed != EntityId::Mu())
  {
    Inventory& inventory = pImpl->viewed->getInventory();

    for (auto iter = std::begin(pImpl->selected_slots);
         iter != std::end(pImpl->selected_slots);
         ++iter)
    {
      EntityId entity = inventory[*iter];
      entities.push_back(entity);
    }
  }

  return entities;
}

void InventorySelection::clear_selected_slots()
{
  pImpl->selected_slots.clear();
  broadcast(EventSelectionChanged(pImpl->selected_slots, pImpl->selected_quantity));
}

unsigned int InventorySelection::get_selected_quantity() const
{
  return pImpl->selected_quantity;
}

unsigned int InventorySelection::get_max_quantity() const
{
  unsigned int result;

  if (pImpl->viewed == EntityId::Mu())
  {
    return 0;
  }

  Inventory& inventory = pImpl->viewed->getInventory();

  if (pImpl->selected_slots.size() == 0)
  {
    result = 0;
  }
  else
  {
    EntityId entity = inventory[pImpl->selected_slots[0]];

    if (entity == EntityId::Mu())
    {
      result = 0;
    }
    else
    {
      result = COMPONENTS.physical.value(entity).quantity();
    }
  }

  return result;
}

unsigned int InventorySelection::reset_selected_quantity()
{
  pImpl->selected_quantity = get_max_quantity();
  broadcast(EventSelectionChanged(pImpl->selected_slots, pImpl->selected_quantity));
  return pImpl->selected_quantity;
}

bool InventorySelection::set_selected_quantity(unsigned int amount)
{
  if (amount > 0)
  {
    unsigned int maximum = get_max_quantity();
    if (amount <= maximum)
    {
      pImpl->selected_quantity = amount;
      broadcast(EventSelectionChanged(pImpl->selected_slots, pImpl->selected_quantity));
      return true;
    }
  }
  return false;
}

bool InventorySelection::inc_selected_quantity()
{
  unsigned int maximum = get_max_quantity();
  if (pImpl->selected_quantity < maximum)
  {
    ++(pImpl->selected_quantity);
    broadcast(EventSelectionChanged(pImpl->selected_slots, pImpl->selected_quantity));
    return true;
  }
  return false;
}

bool InventorySelection::dec_selected_quantity()
{
  if (pImpl->selected_quantity > 1)
  {
    --(pImpl->selected_quantity);
    broadcast(EventSelectionChanged(pImpl->selected_slots, pImpl->selected_quantity));
    return true;
  }
  return false;
}

EntityId InventorySelection::getEntity(InventorySlot selection)
{
  EntityId viewed = pImpl->viewed;

  if (viewed == EntityId::Mu())
  {
    return EntityId::Mu();
  }

  Inventory& inventory = pImpl->viewed->getInventory();

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

char InventorySelection::get_character(InventorySlot slot)
{
  return get_character(static_cast<unsigned int>(slot));
}

char InventorySelection::get_character(unsigned int slot_number)
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

InventorySlot InventorySelection::get_slot(char character)
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

std::unordered_set<EventID> InventorySelection::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  events.insert(
  {
    EventEntityChanged::id(),
    EventSelectionChanged::id()
  });
  return events;
}
