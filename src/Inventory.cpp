#include "Inventory.h"

#include <algorithm>
#include <climits>

#include "Entity.h"
#include "ErrorHandler.h"
#include "Thing.h"
#include "ThingFactory.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
  //dtor
}

bool Inventory::add(std::shared_ptr<Thing> thing)
{
  // If pointer is null, exit returning false.
  if (!thing)
  {
    return false;
  }

  // If the thing is the player, it goes into slot 0.
  if (thing->is_player())
  {
    if (things_.count(INVSLOT_ZERO) != 0)
    {
      /// @todo Move anything in this slot to a new slot.  This will be required
      ///       if it's possible to change the ID of the player Entity.
      MAJOR_ERROR("Slot 0 of inventory already contains the player!");
    }
    things_[INVSLOT_ZERO] = thing;
    return true;
  }

  auto found_thing = find_ptr(thing);

  if (found_thing == things_.cend())
  {
    for (InventorySlot slot = INVSLOT_MIN; slot < INVSLOT_MAX; ++slot)
    {
      if (things_.count(slot) == 0)
      {
        things_[slot] = thing;
        consolidate_items();
        return true;
      }
    }
  }

  return false;
}

void Inventory::clear()
{
  things_.clear();
}

unsigned int Inventory::count()
{
  return things_.size();
}

ThingMap const& Inventory::get_things()
{
  return things_;
}

void Inventory::consolidate_items()
{
  auto first_iter = std::begin(things_);
  while (first_iter != std::end(things_))
  {
    ++first_iter;
    auto second_iter = first_iter;
    --first_iter;

    while (second_iter != std::end(things_))
    {
      Thing& first_thing = *(first_iter->second.get());
      Thing& second_thing = *(second_iter->second.get());

      if (first_thing.can_merge_with(second_thing))
      {
        auto first_quantity = first_thing.get_quantity();
        auto second_quantity = second_thing.get_quantity();
        first_thing.set_quantity(first_quantity + second_quantity);

        auto second_iter_copy = second_iter;
        --second_iter;
        things_.erase(second_iter_copy);
      }
      ++second_iter;
    }
    ++first_iter;
  }
}

bool Inventory::contains(std::weak_ptr<Thing> thing)
{
  auto locked_thing = thing.lock();
  if (locked_thing == nullptr) return false;

  return (find_ptr(locked_thing) != things_.cend());
}

bool Inventory::contains(Thing& thing)
{
  return (find_ref(thing) != things_.cend());
}

bool Inventory::contains(InventorySlot slot)
{
  return (things_.count(slot) != 0);
}

InventorySlot Inventory::get(std::weak_ptr<Thing> thing)
{
  auto locked_thing = thing.lock();
  if (locked_thing == nullptr) return INVSLOT_INVALID;

  auto iter = find_ptr(locked_thing);
  if (iter != things_.cend())
  {
    return iter->first;
  }

  return INVSLOT_INVALID;
}

std::weak_ptr<Thing> Inventory::get(InventorySlot slot)
{
  return (things_.at(slot));
}

std::shared_ptr<Thing> Inventory::split(Thing& thing,
                                        unsigned int target_quantity)
{
  std::shared_ptr<Thing> source_thing;
  std::shared_ptr<Thing> target_thing;
  unsigned int source_quantity;

  if (target_quantity > 0)
  {
    auto iter = find_ref(thing);
    if (iter != things_.cend())
    {
      source_thing = iter->second;
      source_quantity = source_thing->get_quantity();
      if (target_quantity < source_quantity)
      {
        target_thing = source_thing->clone();
        source_thing->set_quantity(source_quantity - target_quantity);
        target_thing->set_quantity(target_quantity);
      }
    }
  }
  return target_thing;
}

std::shared_ptr<Thing> Inventory::remove(InventorySlot slot)
{
  std::shared_ptr<Thing> removed_thing;
  if (things_.count(slot) != 0)
  {
    removed_thing = things_[slot];
    things_.erase(slot);
  }
  return removed_thing;
}

std::shared_ptr<Thing> Inventory::remove(Thing& thing)
{
  std::shared_ptr<Thing> removed_thing;

  auto iter = find_ref(thing);
  if (iter != things_.cend())
  {
    removed_thing = iter->second;
    things_.erase(iter);
  }
  return removed_thing;
}

std::weak_ptr<Thing> Inventory::get_largest_thing()
{
  auto iter_largest = things_.cbegin();

  for (ThingMap::const_iterator iter = things_.cbegin();
        iter != things_.cend(); ++iter)
  {
    if (is_smaller_than(iter_largest->second, iter->second))
    {
      iter_largest = iter;
    }
  }
  return iter_largest->second;
}

ThingMap::iterator Inventory::find_ptr(std::shared_ptr<Thing> target)
{
  ThingMap::iterator iter = find_if(things_.begin(),
                                    things_.end(),
                                    [&](ThingPair const& thing_pair)
                                    {
                                        return thing_pair.second.get() ==
                                               target.get();
                                    });
  return iter;
}

ThingMap::iterator Inventory::find_ref(Thing& target)
{
  ThingMap::iterator iter = find_if(things_.begin(),
                                    things_.end(),
                                    [&](ThingPair const& thing_pair)
                                    {
                                        return thing_pair.second.get() ==
                                               &target;
                                    });
  return iter;
}

bool Inventory::is_smaller_than(std::shared_ptr<Thing> const& a, std::shared_ptr<Thing> const& b)
{
  if (!a || !b) return false;

  return (a->get_mass() < b->get_mass());
}
