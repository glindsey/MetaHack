#ifndef INVENTORY_H
#define INVENTORY_H

#include "stdafx.h"

#include "InventorySlot.h"
#include "EntityId.h"

/// Forward declarations
class Entity;

// Using std::map here because we DO need to iterate over a sorted list often
typedef std::map< InventorySlot, EntityId > EntityMap;
typedef std::pair< InventorySlot, EntityId > EntityPair;

/// An Inventory is a collection of Entities.  It has an unchangeable owner.
class Inventory
{
public:
  Inventory();
  virtual ~Inventory();

  /// Adds the passed Entity to the inventory.
  /// @param entity Entity to add to the inventory.
  /// @return True if the add succeeded; false otherwise.
  bool add(EntityId entity);

  /// Clears this inventory.
  void clear();

  /// Gets the current count of this inventory.
  /// @return The number of items in the inventory.
  size_t count();

  /// Gets a beginning iterator to the entities map.
  EntityMap::iterator begin();

  /// Gets an ending iterator to the entities map.
  EntityMap::iterator end();

  /// Gets a beginning const iterator to the entities map.
  EntityMap::const_iterator cbegin();

  /// Gets an ending const iterator to the entities map.
  EntityMap::const_iterator cend();

  /// Finds items with identical qualities and combines them into a single
  /// aggregate item.
  void consolidate_items();

  bool contains(EntityId entity);
  bool contains(InventorySlot slot);

  InventorySlot operator[](EntityId entity);

  EntityId operator[](InventorySlot slot);

  /// Splits an inventory item with a quantity > 1 into two items.
  /// This function clones the item in question, and splits the
  /// quantity variable between the two of them. It then returns
  /// the newly cloned item as a shared pointer. This is used when
  /// a player needs to perform an action on a portion of an item group
  /// (such as dropping 50 gold coins).
  /// The newly created object is <i>not</i> included in the inventory.
  /// @param entity Reference to the entity to split.
  /// @param target_quantity Target quantity to split out.
  /// @return The UUID of the new Entity.
  EntityId split(EntityId entity, unsigned int target_quantity);

  EntityId remove(EntityId entity);

  EntityId remove(InventorySlot slot);

  EntityId get_largest_thing();
  EntityId getEntity();

protected:
  EntityMap::iterator find(EntityId target);
  EntityMap::iterator find_if(std::function<bool(EntityPair const&)> functor);

  static bool is_smaller_than(EntityId a, EntityId b);

private:
  /// Entities contained in this Inventory, using slot number as key.
  EntityMap things_;
};

#endif // INVENTORY_H
