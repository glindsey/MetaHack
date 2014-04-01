#ifndef INVENTORYSLOT_H_INCLUDED
#define INVENTORYSLOT_H_INCLUDED

#include <boost/strong_typedef.hpp>

/// ID of a slot in an Inventory.
BOOST_STRONG_TYPEDEF(int, InventorySlot)
#define INVSLOT_INVALID (static_cast<InventorySlot>(-1))
#define INVSLOT_ZERO (static_cast<InventorySlot>(0))
#define INVSLOT_MIN (static_cast<InventorySlot>(1))
#define INVSLOT_MAX (static_cast<InventorySlot>(USHRT_MAX))

#endif // INVENTORYSLOT_H_INCLUDED
