#ifndef THINGIMPL_H
#define THINGIMPL_H

#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

#include "common_types.h"
#include "AttributeSet.h"
#include "Gender.h"
#include "MapTile.h"
#include "ThingMetadata.h"
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

  // Constructor for thing of certain type.
  ThingImpl(std::string type_, ThingRef ref_)
    : 
    type{ type_ },
    ref{ ref_ },
    metadata{ ThingMetadata::get(type_) },
    location{ TM.get_mu() },
    map_tile{ nullptr },
    inventory{ Inventory() },
    quantity{ 1 },
    attributes{ AttributeSet() },
    gender{ Gender::None },
    map_memory{ MapMemory() },
    tiles_currently_seen{ TilesSeen() },
    actions{ ActionQueue() },
    wielded_items{ WieldingMap() },
    equipped_items{ WearingMap() }
  {
    // Set properties to the type defaults.
    property_flags = metadata.get_default_flags();
    property_strings = metadata.get_default_strings();
    property_values = metadata.get_default_values();
  }

  // Constructor for floor of a MapTile.
  ThingImpl(MapTile* tile, ThingRef ref_)
    :
    type{ "floor" },
    ref{ ref_ },
    metadata{ ThingMetadata::get("floor") },
    location{ TM.get_mu() },
    map_tile{ tile },
    inventory{ Inventory() },
    quantity{ 1 },
    attributes{ AttributeSet() },
    gender{ Gender::None },
    map_memory{ MapMemory() },
    tiles_currently_seen{ TilesSeen() },
    actions{ ActionQueue() },
    wielded_items{ WieldingMap() },
    equipped_items{ WearingMap() }
  {
    // Set properties to the type defaults.
    property_flags = metadata.get_default_flags();
    property_strings = metadata.get_default_strings();
    property_values = metadata.get_default_values();
  }

  // Clone constructor.
  ThingImpl(ThingImpl const& other, ThingRef ref_)
    :
    type{ other.type },
    ref{ ref_ },
    metadata{ other.metadata },
    location{ other.location },
    map_tile{ other.map_tile },
    inventory{ Inventory() },             // don't copy
    quantity{ other.quantity },
    property_flags{ other.property_flags },
    property_strings{ other.property_strings },
    property_values{ other.property_values },
    attributes{ other.attributes },
    gender{ other.gender },
    map_memory{ other.map_memory },
    tiles_currently_seen{ TilesSeen() },  // don't copy
    actions{ ActionQueue() },             // don't copy
    wielded_items{ WieldingMap() },       // don't copy
    equipped_items{ WearingMap() }        // don't copy
  {}

  ~ThingImpl()
  {
    property_flags.clear();
    property_values.clear();
    property_strings.clear();
    map_memory.clear();
    tiles_currently_seen.clear();
    actions.clear();
  }

  /// This Thing's type.
  std::string type;

  /// Reference to this Thing.
  ThingRef ref;

  /// Reference to this Thing's metadata.
  ThingMetadata& metadata;

  /// Reference to this Thing's location.
  ThingRef location;

  /// If this Thing is a Floor, pointer to the MapTile it is on.
  MapTile* map_tile;

  Inventory inventory;

  unsigned int quantity;

  /// Map of property flags.
  FlagsMap property_flags;

  /// Map of property values.
  ValuesMap property_values;

  /// Map of property strings.
  StringsMap property_strings;

  /// Entity's attributes.
  AttributeSet attributes;

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

  /// Queue of actions to be performed.
  ActionQueue actions;

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
