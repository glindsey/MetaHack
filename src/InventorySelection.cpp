#include "stdafx.h"

#include "InventorySelection.h"

#include "App.h"
#include "Inventory.h"
#include "Thing.h"
#include "ThingId.h"

struct InventorySelection::Impl
{
  /// Thing whose contents (or surroundings) are currently being viewed.
  ThingId viewed;

  /// Vector of selected inventory slots.
  std::vector< InventorySlot > selected_slots;

  /// Reference to quantity of topmost selected item.
  unsigned int selected_quantity;


};

InventorySelection::InventorySelection()
  :
  pImpl(NEW Impl())
{
  SET_UP_LOGGER("InventorySelection", true);
}

InventorySelection::InventorySelection(ThingId thing)
  :
  InventorySelection()
{
  set_viewed(thing);
}

InventorySelection::~InventorySelection()
{
  //dtor
}

ThingId InventorySelection::get_viewed() const
{
  return pImpl->viewed;
}

void InventorySelection::set_viewed(ThingId thing)
{
  pImpl->viewed = thing;
  pImpl->selected_slots.clear();
  notifyObservers(Event::Updated);
}

void InventorySelection::toggle_selection(InventorySlot selection)
{
  if (pImpl->viewed == ThingId::Mu())
  {
    return;
  }

  Inventory& inventory = pImpl->viewed->get_inventory();

  if (inventory.contains(selection))
  {
    auto iter = std::find(std::begin(pImpl->selected_slots),
                          std::end(pImpl->selected_slots),
                          selection);
    if (iter == std::end(pImpl->selected_slots))
    {
      CLOG(TRACE, "InventorySelection") <<
        "Adding slot " << static_cast<unsigned int>(selection) <<
        "to selected things";
      pImpl->selected_slots.push_back(selection);
    }
    else
    {
      CLOG(TRACE, "InventorySelection") <<
        "Removing slot " << static_cast<unsigned int>(selection) <<
        "from selected things";
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

std::vector<ThingId> InventorySelection::get_selected_things()
{
  std::vector<ThingId> things;

  if (pImpl->viewed != ThingId::Mu())
  {
    Inventory& inventory = pImpl->viewed->get_inventory();

    for (auto iter = std::begin(pImpl->selected_slots);
         iter != std::end(pImpl->selected_slots);
         ++iter)
    {
      ThingId thing = inventory[*iter];
      things.push_back(thing);
    }
  }

  return things;
}

void InventorySelection::clear_selected_slots()
{
  pImpl->selected_slots.clear();
  notifyObservers(Event::Updated);
}

unsigned int InventorySelection::get_selected_quantity() const
{
  return pImpl->selected_quantity;
}

unsigned int InventorySelection::get_max_quantity() const
{
  unsigned int result;

  if (pImpl->viewed == ThingId::Mu())
  {
    return 0;
  }

  Inventory& inventory = pImpl->viewed->get_inventory();

  if (pImpl->selected_slots.size() == 0)
  {
    result = 0;
  }
  else
  {
    ThingId thing = inventory[pImpl->selected_slots[0]];

    if (thing == ThingId::Mu())
    {
      result = 0;
    }
    else
    {
      result = thing->get_quantity();
    }
  }

  return result;
}

unsigned int InventorySelection::reset_selected_quantity()
{
  pImpl->selected_quantity = get_max_quantity();
  notifyObservers(Event::Updated);
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
      notifyObservers(Event::Updated);
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
    notifyObservers(Event::Updated);
    return true;
  }
  return false;
}

bool InventorySelection::dec_selected_quantity()
{
  if (pImpl->selected_quantity > 1)
  {
    --(pImpl->selected_quantity);
    notifyObservers(Event::Updated);
    return true;
  }
  return false;
}

ThingId InventorySelection::get_thing(InventorySlot selection)
{
  ThingId viewed = pImpl->viewed;

  if (viewed == ThingId::Mu())
  {
    return ThingId::Mu();
  }

  Inventory& inventory = pImpl->viewed->get_inventory();

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
    return ThingId::Mu();
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

  ASSERT_CONDITION(((char_number >= 0x40) && (char_number <= 0x5a)) ||
    ((char_number >= 0x61) && (char_number <= 0x7a)));

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