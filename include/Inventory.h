#ifndef INVENTORY_H
#define INVENTORY_H

#include <map>
#include <memory>
#include <boost/uuid/uuid.hpp>

#include "InventorySlot.h"
#include "ThingRef.h"

/// Forward declarations
class Thing;

// Using Declarations
using UUID = boost::uuids::uuid;

// Using std::map here because we DO need to iterate over a sorted list often
typedef std::map< InventorySlot, ThingRef > ThingMap;
typedef std::pair< InventorySlot, ThingRef > ThingPair;

/// An Inventory is a collection of Things.  It has an unchangeable owner.
class Inventory
{
public:
  Inventory();
  virtual ~Inventory();

  /// Adds the passed Thing to the inventory.
  /// @param thing Thing to add to the inventory.
  /// @return True if the add succeeded; false otherwise.
  bool add(ThingRef thing);

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

  bool contains(ThingRef thing);
  bool contains(InventorySlot slot);

  InventorySlot get(ThingRef thing);

  ThingRef get(InventorySlot slot);

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
  ThingRef split(ThingRef thing, unsigned int target_quantity);

  ThingRef remove(ThingRef thing);

  ThingRef remove(InventorySlot slot);

  ThingRef get_largest_thing();
  ThingRef get_entity();

protected:
  ThingMap::iterator find(ThingRef target);
  ThingMap::iterator find_if(std::function<bool(ThingPair const&)> functor);

  static bool is_smaller_than(ThingRef a, ThingRef b);

private:
  /// Things contained in this Inventory, using slot number as key.
  ThingMap things_;
};

#endif // INVENTORY_H
