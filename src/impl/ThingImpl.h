#ifndef THINGIMPL_H
#define THINGIMPL_H

#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

#include "common_types.h"
#include "Gender.h"
#include "MapTile.h"
#include "Metadata.h"
#include "PropertyDictionary.h"
#include "ThingRef.h"

// Using declarations.
using WieldingMap = std::unordered_map<unsigned int, ThingRef>;
using WieldingPair = std::pair<unsigned int, ThingRef>;

using WearingMap = std::unordered_map<WearLocation, ThingRef>;
using WearingPair = std::pair<WearLocation, ThingRef>;

using MapMemory = std::vector<std::string>;
using TilesSeen = boost::dynamic_bitset<>;
using ActionQueue = std::deque<Action>;

class ThingImpl
{
public:

  /// Constructor for thing of certain type.
  ThingImpl(Metadata& metadata_, ThingRef ref_)
    : 
    metadata{ metadata_ },
    ref{ ref_ },
    location{ TM.get_mu() },
    map_tile{ nullptr },
    inventory{ Inventory() },
    gender{ Gender::None },
    map_memory{ MapMemory() },
    tiles_currently_seen{ TilesSeen() },
    pending_actions{ ActionQueue() },
    wielded_items{ WieldingMap() },
    equipped_items{ WearingMap() }
  {
    // Properties can remain clear; if a property is ever missing, we
    // populate it with the default from the ThingMetadata (if one exists).
  }

  /// Constructor for floor of a MapTile.
  ThingImpl(MapTile* tile, Metadata& metadata_, ThingRef ref_)
    :
    metadata{ metadata_ },
    ref{ ref_ },
    location{ TM.get_mu() },
    map_tile{ tile },
    inventory{ Inventory() },
    gender{ Gender::None },
    map_memory{ MapMemory() },
    tiles_currently_seen{ TilesSeen() },
    pending_actions{ ActionQueue() },
    wielded_items{ WieldingMap() },
    equipped_items{ WearingMap() }
  {
    // Properties can remain clear; if a property is ever missing, we
    // populate it with the default from the ThingMetadata (if one exists).
  }

  /// Clone constructor.
  ThingImpl(ThingImpl const& other, ThingRef ref_)
    :
    metadata{ other.metadata },
    ref{ ref_ },
    location{ other.location },
    map_tile{ other.map_tile },
    inventory{ Inventory() },             // don't copy
    properties{ other.properties },
    gender{ other.gender },
    map_memory{ other.map_memory },
    tiles_currently_seen{ TilesSeen() },  // don't copy
    pending_actions{ ActionQueue() },             // don't copy
    wielded_items{ WieldingMap() },       // don't copy
    equipped_items{ WearingMap() }        // don't copy
  {}

  ~ThingImpl()
  {
    map_memory.clear();
    tiles_currently_seen.clear();
    pending_actions.clear();
  }

  /// Reference to this Thing's metadata.
  Metadata& metadata;

  /// Reference to this Thing.
  ThingRef ref;

  /// Reference to this Thing's location.
  ThingRef location;

  /// If this Thing is a Floor, pointer to the MapTile it is on.
  MapTile* map_tile;

  Inventory inventory;

  /// Property dictionary.
  PropertyDictionary properties;

  /// Gender of this entity.
  Gender gender = Gender::None;

  /// Entity's spacial memory of map tiles.
  /// @todo Regarding memory, it would be AWESOME if it could fade out
  ///       VERY gradually, over a long period of time. Seeing it would
  ///       reset the memory counter to 0, or possibly just add a large
  ///       amount to the counter so that places you see more frequently
  ///       stay in your mind longer.
  MapMemory map_memory;

  /// Bitset for tiles currently seen.
  /// This deals with tiles observed at this particular instant.
  TilesSeen tiles_currently_seen;

  /// Queue of pending_actions to be performed.
  ActionQueue pending_actions;

  /// Map of items wielded.
  WieldingMap wielded_items;

  /// Map of things worn.
  WearingMap equipped_items;

  bool is_wielding(ThingRef thing, unsigned int& hand)
  {
    if (thing == TM.get_mu())
    {
      return false;
    }
    auto found_item = 
      std::find_if(wielded_items.cbegin(),
        wielded_items.cend(),
        [&](WieldingPair const& p)
    { return p.second == thing; });

    if (found_item == wielded_items.cend())
    {
      return false;
    }
    else
    {
      hand = found_item->first;
      return true;
    }
  }

  ThingRef wielding_in(unsigned int hand)
  {
    if (wielded_items.count(hand) == 0)
    {
      return TM.get_mu();
    }
    else
    {
      return wielded_items[hand];
    }
  }

  bool is_wearing(ThingRef thing, WearLocation& location)
  {
    if (thing == TM.get_mu())
    {
      return false;
    }
    auto found_item = 
      std::find_if(equipped_items.cbegin(),
        equipped_items.cend(),
        [&](WearingPair const& p)
    { return p.second == thing; });

    if (found_item == equipped_items.cend())
    {
      return false;
    }
    else
    {
      location = found_item->first;
      return true;
    }
  }

  void do_wield(ThingRef thing, unsigned int hand)
  {
    wielded_items[hand] = thing;
  }

  bool do_unwield(ThingRef thing)
  {
    unsigned int hand;
    if (is_wielding(thing, hand) == false)
    {
      return false;
    }
    else
    {
      wielded_items.erase(hand);
      return true;
    }
  }

  bool do_unwield(unsigned int hand)
  {
    if (wielded_items.count(hand) == 0)
    {
      return false;
    }
    else
    {
      wielded_items.erase(hand);
      return true;
    }
  }

  bool do_equip(ThingRef thing, WearLocation location)
  {
    if (equipped_items.count(location) == 0)
    {
      equipped_items[location] = thing;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool do_deequip(ThingRef thing)
  {
    WearLocation location;
    if (is_wearing(thing, location) == false)
    {
      return false;
    }
    else
    {
      equipped_items.erase(location);
      return true;
    }
  }
};



#endif // THINGIMPL_H
