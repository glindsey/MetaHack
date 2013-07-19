#include "Inventory.h"

#include <algorithm>
#include <climits>

#include "ErrorHandler.h"
#include "Thing.h"
#include "ThingFactory.h"
#include "ThingId.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
  //dtor
}

bool Inventory::add(ThingId id)
{
  // If the thing is the player, it goes into slot 0.
  if (id == TF.get_player_id())
  {
    if (things_by_slot_.count(INVSLOT_ZERO) != 0)
    {
      /// @todo Move anything in this slot to a new slot.  This will be required
      ///       if it's possible to change the ID of the player Entity.
      MAJOR_ERROR("Slot 0 of inventory already contains the player!");
    }
    things_by_id_[id] = INVSLOT_ZERO;
    things_by_slot_[INVSLOT_ZERO] = id;
    return true;
  }

  if (things_by_id_.count(id) == 0)
  {
    for (InventorySlot slot = INVSLOT_MIN; slot < INVSLOT_MAX; ++slot)
    {
      if (things_by_slot_.count(slot) == 0)
      {
        things_by_id_[id] = slot;
        things_by_slot_[slot] = id;
        return true;
      }
    }
  }

  return false;
}

void Inventory::clear()
{
  things_by_id_.clear();
  things_by_slot_.clear();
}

unsigned int Inventory::count()
{
  return things_by_id_.size();
}

ThingMapById::iterator Inventory::by_id_begin()
{
  return things_by_id_.begin();
}

ThingMapById::iterator Inventory::by_id_end()
{
  return things_by_id_.end();
}

ThingMapById::const_iterator Inventory::by_id_cbegin()
{
  return things_by_id_.cbegin();
}

ThingMapById::const_iterator Inventory::by_id_cend()
{
  return things_by_id_.cend();
}

ThingMapBySlot::iterator Inventory::by_slot_begin()
{
  return things_by_slot_.begin();
}

ThingMapBySlot::iterator Inventory::by_slot_end()
{
  return things_by_slot_.end();
}

ThingMapBySlot::const_iterator Inventory::by_slot_cbegin()
{
  return things_by_slot_.cbegin();
}

ThingMapBySlot::const_iterator Inventory::by_slot_cend()
{
  return things_by_slot_.cend();
}

bool Inventory::contains(ThingId id)
{
  return (things_by_id_.count(id) != 0);
}

bool Inventory::contains(InventorySlot slot)
{
  return (things_by_slot_.count(slot) != 0);
}

InventorySlot Inventory::get(ThingId id)
{
  return (things_by_id_.at(id));
}

ThingId Inventory::get(InventorySlot slot)
{
  return (things_by_slot_.at(slot));
}

bool Inventory::remove(ThingId id)
{
  if (things_by_id_.count(id) != 0)
  {
    InventorySlot slot = things_by_id_[id];
    things_by_id_.erase(id);
    things_by_slot_.erase(slot);
    return true;
  }
  return false;
}

ThingId Inventory::get_largest_thing_id()
{
  ThingId largest = (*(things_by_id_.cbegin())).first;

  for (ThingMapById::const_iterator iter = things_by_id_.cbegin();
        iter != things_by_id_.cend(); ++iter)
  {
    if (is_smaller_than(largest, (*iter).first))
    {
      largest = (*iter).first;
    }
  }
  return largest;
}

bool Inventory::is_smaller_than(ThingId a, ThingId b)
{
  Thing const& thingA = TF.get(a);
  Thing const& thingB = TF.get(b);

  return (thingA.get_size() < thingB.get_size());
}
