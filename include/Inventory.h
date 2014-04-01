#ifndef INVENTORY_H
#define INVENTORY_H

#include <map>
#include <memory>

#include "InventorySlot.h"

/// Forward declarations
class Thing;

typedef std::map< InventorySlot, std::shared_ptr<Thing> > ThingMap;
typedef std::pair< InventorySlot, std::shared_ptr<Thing> > ThingPair;

/// An Inventory is a collection of Things.  It has an unchangeable owner.
class Inventory
{
  public:
    Inventory();
    virtual ~Inventory();

    /// Adds the passed Thing to the inventory.
    /// @param thing Thing to add to the inventory.
    /// @return True if the add succeeded; false otherwise.
    bool add(std::shared_ptr<Thing> thing);

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

    bool contains(std::weak_ptr<Thing> thing);
    bool contains(Thing& thing);
    bool contains(InventorySlot slot);

    InventorySlot get(std::weak_ptr<Thing> thing);

    std::weak_ptr<Thing> get(InventorySlot slot);

    /// Splits an inventory item with a quantity > 1 into two items.
    /// This function clones the item in question, and splits the
    /// quantity variable between the two of them. It then returns
    /// the newly cloned item as a shared pointer. This is used when
    /// a player needs to perform an action on a portion of an item group
    /// (such as dropping 50 gold coins).
    /// The newly created object is <i>not</i> included in the inventory;
    /// if it goes out of scope, it <i>will</i> be destroyed.
    /// @param thing The thing to split.
    /// @param target_quantity Target quantity to split out.
    /// @return A shared_ptr to the new Thing.
    /// @note The target quantity must be between 1 and source quantity - 1.
    ///       If it isn't, no split is performed and an empty shared_ptr is
    ///       returned.
    std::shared_ptr<Thing> split(Thing& thing,
                                 unsigned int target_quantity);

    std::shared_ptr<Thing> remove(Thing& thing);

    std::shared_ptr<Thing> remove(InventorySlot slot);

    std::weak_ptr<Thing> get_largest_thing();

  protected:
    ThingMap::iterator find_ptr(std::shared_ptr<Thing> target);
    ThingMap::iterator find_ref(Thing& target);

    static bool is_smaller_than(std::shared_ptr<Thing> const& a,
                                std::shared_ptr<Thing> const& b);

  private:
    /// Things contained in this Inventory, using slot number as key.
    ThingMap things_;
};

#endif // INVENTORY_H
