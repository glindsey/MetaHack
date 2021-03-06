#pragma once

#include <map>
#include <utility>

#include "inventory/InventorySlot.h"
#include "entity/EntityId.h"

namespace Components
{

  /// Forward declarations
  class Entity;

  // Using std::map here because we DO need to iterate over a sorted list often
  using EntityMap = std::map<InventorySlot, EntityId>;
  using EntityPair = std::pair<InventorySlot, EntityId>;

  /// An Inventory is a collection of Entities.  It has an unchangeable owner.
  class ComponentInventory
  {
  public:
    ComponentInventory();
    virtual ~ComponentInventory();

    friend void from_json(json const& j, ComponentInventory& obj);
    friend void to_json(json& j, ComponentInventory const& obj);

    /// Adds the passed Entity to the inventory.
    /// @param entity Entity to add to the inventory.
    /// @return True if the add succeeded; false otherwise.
    bool add(EntityId entity, bool isPlayer);

    /// Clears this inventory.
    void clear();

    /// Gets the maximum allowed size of this inventory.
    size_t const& maxSize() const;

    /// Gets the current count of this inventory.
    /// @return The number of items in the inventory.
    size_t count();

    /// Gets a vector representing the entities map.
    /// @todo This is temporary until I get rid of InventorySlots altogether.
    std::vector<EntityId> getCollection();

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
    void consolidateItems();

    bool can_merge(EntityId first, EntityId second) const;

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
    /// @todo This doesn't really belong here, should be in its own System.
    EntityId split(EntityFactory& entities, EntityId entity, unsigned int target_quantity);

    EntityId remove(EntityId entity);

    EntityId remove(InventorySlot slot);

    EntityId getLargestEntity();
    EntityId getEntity();

    /// Returns whether this inventory can hold the specified entity.
    bool canContain(EntityId entity);

  protected:
    EntityMap::iterator find(EntityId target);
    EntityMap::iterator find_if(std::function<bool(EntityPair const&)> functor);

    static bool isSmallerThan(EntityId a, EntityId b);

  private:
    /// Entities contained in this Inventory, using slot number as key.
    EntityMap m_entities;

    /// Maximum allowed size for this Inventory.
    size_t m_maxSize = std::numeric_limits<size_t>::max();
  };

} // end namespace Components
