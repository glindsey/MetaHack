#ifndef INVENTORY_H
#define INVENTORY_H

#include "stdafx.h"

#include "InventorySlot.h"
#include "ThingId.h"

/// Forward declarations
class Thing;

// Using std::map here because we DO need to iterate over a sorted list often
typedef std::map< InventorySlot, ThingId > ThingMap;
typedef std::pair< InventorySlot, ThingId > ThingPair;

/// An Inventory is a collection of Things.  It has an unchangeable owner.
class Inventory
{
public:
  Inventory();
  virtual ~Inventory();

  /// Adds the passed Thing to the inventory.
  /// @param thing Thing to add to the inventory.
  /// @return True if the add succeeded; false otherwise.
  bool add(ThingId thing);

  /// Clears this inventory.
  void clear();

  /// Gets the current count of this inventory.
  /// @return The number of items in the inventory.
  unsigned int count();

  /// Gets a map of all of the things in the inventory.
  /// @return A map associating InventorySlot objects to
  ///         std::shared_ptr<Thing> pointers.
  /// @warning This function exposes too much of the class internals
  ///          to the outside, and I may decide to refactor it later.
  ThingMap const& get_things();

  /// Finds items with identical qualities and combines them into a single
  /// aggregate item.
  void consolidate_items();

  bool contains(ThingId thing);
  bool contains(InventorySlot slot);

  InventorySlot get(ThingId thing);

  ThingId get(InventorySlot slot);

  /// Splits an inventory item with a quantity > 1 into two items.
  /// This function clones the item in question, and splits the
  /// quantity variable between the two of them. It then returns
  /// the newly cloned item as a shared pointer. This is used when
  /// a player needs to perform an action on a portion of an item group
  /// (such as dropping 50 gold coins).
  /// The newly created object is <i>not</i> included in the inventory.
  /// @param thing Reference to the thing to split.
  /// @param target_quantity Target quantity to split out.
  /// @return The UUID of the new Thing.
  ThingId split(ThingId thing, unsigned int target_quantity);

  ThingId remove(ThingId thing);

  ThingId remove(InventorySlot slot);

  ThingId get_largest_thing();
  ThingId get_entity();

protected:
  ThingMap::iterator find(ThingId target);
  ThingMap::iterator find_if(std::function<bool(ThingPair const&)> functor);

  static bool is_smaller_than(ThingId a, ThingId b);

private:
  /// Things contained in this Inventory, using slot number as key.
  ThingMap things_;
};

#endif // INVENTORY_H
