#ifndef INVENTORY_H
#define INVENTORY_H

#include <map>
#include <memory>

#include "InventorySlot.h"
#include "ThingId.h"

/// Forward declarations
class Thing;

typedef std::map<ThingId, InventorySlot> ThingMapById;
typedef std::map<InventorySlot, ThingId> ThingMapBySlot;

/// An Inventory is a collection of Things.  It has an unchangable owner.
class Inventory
{
  public:
    Inventory();
    virtual ~Inventory();

    bool add(ThingId thing);

    void clear();

    unsigned int count();

    ThingMapById::iterator by_id_begin();

    ThingMapById::iterator by_id_end();

    ThingMapById::const_iterator by_id_cbegin();

    ThingMapById::const_iterator by_id_cend();

    ThingMapBySlot::iterator by_slot_begin();

    ThingMapBySlot::iterator by_slot_end();

    ThingMapBySlot::const_iterator by_slot_cbegin();

    ThingMapBySlot::const_iterator by_slot_cend();

    bool contains(ThingId id);

    bool contains(InventorySlot slot);

    InventorySlot get(ThingId id);

    ThingId get(InventorySlot slot);

    bool remove(ThingId id);

    bool remove(InventorySlot slot);

    ThingId get_largest_thing_id();

  protected:
    static bool is_smaller_than(ThingId a, ThingId b);

  private:
    /// Things contained in this Inventory, using ThingIds as key.
    ThingMapById things_by_id_;

    /// Things contained in this Inventory, using slot number as key.
    ThingMapBySlot things_by_slot_;
};

#endif // INVENTORY_H
