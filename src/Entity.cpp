#include "stdafx.h"

#include "Entity.h"

#include "App.h"
#include "Direction.h"
#include "GameState.h"
#include "Gender.h"
#include "IConfigSettings.h"
#include "IStringDictionary.h"
#include "IntegerRange.h"
#include "Inventory.h"
#include "Map.h"
#include "MapTile.h"
#include "MapTileMetadata.h"
#include "MathUtils.h"
#include "MessageLog.h"
#include "Metadata.h"
#include "Ordinal.h"
#include "Service.h"
#include "EntityPool.h"
#include "TileSheet.h"

// Static member initialization.
sf::Color const Entity::wall_outline_color_ = sf::Color(255, 255, 255, 64);

Entity::Entity(GameState& game, Metadata& metadata, EntityId ref)
  :
  m_game{ game },
  m_metadata{ metadata },
  m_properties{},
  m_ref{ ref },
  m_location{ EntityId::Mu() },
  m_map_tile{ nullptr },
  m_inventory{ Inventory() },
  m_gender{ Gender::None },
  m_map_memory{ MapMemory() },
  m_tiles_currently_seen{ TilesSeen() },
  m_pending_actions{ ActionQueue() },
  m_wielded_items{ WieldingMap() },
  m_equipped_items{ WearingMap() }
{
  initialize();
}

Entity::Entity(GameState& game, MapTile* map_tile, Metadata& metadata, EntityId ref)
  :
  m_game{ game },
  m_metadata{ metadata },
  m_properties{},
  m_ref{ ref },
  m_location{ EntityId::Mu() },
  m_map_tile{ map_tile },
  m_inventory{ Inventory() },
  m_gender{ Gender::None },
  m_map_memory{ MapMemory() },
  m_tiles_currently_seen{ TilesSeen() },
  m_pending_actions{ ActionQueue() },
  m_wielded_items{ WieldingMap() },
  m_equipped_items{ WearingMap() }
{
  initialize();
}

Entity::Entity(Entity const& original, EntityId ref)
  :
  m_game{ original.m_game },
  m_metadata{ original.m_metadata },
  m_properties{ original.m_properties },
  m_ref{ ref },
  m_location{ original.m_location },
  m_map_tile{ original.m_map_tile },
  m_inventory{ Inventory() },             // don't copy
  m_gender{ original.m_gender },
  m_map_memory{ original.m_map_memory },
  m_tiles_currently_seen{ TilesSeen() },  // don't copy
  m_pending_actions{ ActionQueue() },     // don't copy
  m_wielded_items{ WieldingMap() },       // don't copy
  m_equipped_items{ WearingMap() }        // don't copy
{
  initialize();
}

void Entity::initialize()
{
  SET_UP_LOGGER("Entity", true);

  /// Get our maximum HP. (The Lua script will automatically pick it from a range.)
  int max_hp = m_metadata.get_intrinsic<int>("maxhp");
  set_base_property<int>("maxhp", max_hp);

  /// Also set our HP to that value.
  set_base_property<int>("hp", max_hp);

  notifyObservers(Event::Updated);
}

Entity::~Entity()
{
}

void Entity::queue_action(std::unique_ptr<Action> pAction)
{
  m_pending_actions.push_back(std::move(pAction));
}

bool Entity::action_is_pending() const
{
  return !(m_pending_actions.empty());
}

bool Entity::action_is_in_progress()
{
  return (get_base_property<int>("counter_busy") > 0);
}

EntityId Entity::get_wielding_in(unsigned int& hand)
{
  if (m_wielded_items.count(hand) == 0)
  {
    return EntityId::Mu();
  }
  else
  {
    return m_wielded_items[hand];
  }
}

bool Entity::is_wielding(EntityId thing)
{
  unsigned int dummy;
  return is_wielding(thing, dummy);
}

bool Entity::is_wielding(EntityId thing, unsigned int& hand)
{
  if (thing == EntityId::Mu())
  {
    return false;
  }
  auto found_item =
    std::find_if(m_wielded_items.cbegin(),
                 m_wielded_items.cend(),
                 [&](WieldingPair const& p)
  { return p.second == thing; });

  if (found_item == m_wielded_items.cend())
  {
    return false;
  }
  else
  {
    hand = found_item->first;
    return true;
  }
}

bool Entity::has_equipped(EntityId thing)
{
  WearLocation dummy;
  return has_equipped(thing, dummy);
}

bool Entity::has_equipped(EntityId thing, WearLocation& location)
{
  if (thing == EntityId::Mu())
  {
    return false;
  }
  auto found_item =
    std::find_if(m_equipped_items.cbegin(),
                 m_equipped_items.cend(),
                 [&](WearingPair const& p)
  { return p.second == thing; });

  if (found_item == m_equipped_items.cend())
  {
    return false;
  }
  else
  {
    location = found_item->first;
    return true;
  }
}

bool Entity::can_reach(EntityId thing)
{
  // Check if it is our location.
  auto our_location = get_location();
  if (our_location == thing)
  {
    return true;
  }

  // Check if it's at our location.
  auto thing_location = thing->get_location();
  if (our_location == thing_location)
  {
    return true;
  }

  // Check if it's in our inventory.
  if (this->get_inventory().contains(thing))
  {
    return true;
  }

  return false;
}

bool Entity::is_adjacent_to(EntityId thing)
{
  // Get the coordinates we are at.
  MapTile* our_maptile = get_maptile();
  MapTile* thing_maptile = thing->get_maptile();
  if ((our_maptile == nullptr) || (thing_maptile == nullptr))
  {
    return false;
  }

  Vec2i const& our_coords = our_maptile->get_coords();
  Vec2i const& thing_coords = thing_maptile->get_coords();

  return adjacent(our_coords, thing_coords);
}

bool Entity::do_die()
{
  /// @todo Handle stuff like auto-activating life-saving items here.
  /// @todo Pass in the cause of death somehow.

  ActionResult result = perform_action_died();
  std::string message;

  switch (result)
  {
    case ActionResult::Success:
      if (this->is_player())
      {
        message = this->get_you_or_identifying_string() + " die...";
        Service<IMessageLog>::get().add(message);
      }
      else
      {
        bool living = get_modified_property<bool>("living");
        if (living)
        {
          message = this->get_you_or_identifying_string() + " " +
            this->choose_verb("are", "is") + " killed!";
        }
        else
        {
          message = this->get_you_or_identifying_string() + " " +
            this->choose_verb("are", "is") + " destroyed!";
        }
        Service<IMessageLog>::get().add(message);
      }

      // Set the property saying the entity is dead.
      set_base_property<bool>("dead", true);

      // Clear any pending actions.
      m_pending_actions.clear();

      notifyObservers(Event::Updated);
      return true;
    case ActionResult::Failure:
    default:
      message = this->get_you_or_identifying_string() +
        this->choose_verb(" manage", " manages") + " to avoid dying.";
      Service<IMessageLog>::get().add(message);
      return false;
  }
}

ActionResult Entity::can_deequip(EntityId thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing == m_ref)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's already being worn.
  if (!this->has_equipped(thing))
  {
    return ActionResult::FailureItemNotEquipped;
  }

  /// @todo Finish Entity::can_deequip code
  return ActionResult::Success;
}

bool Entity::do_deequip(EntityId thing, unsigned int& action_time)
{
  std::string message;
  ActionResult deequip_try = this->can_deequip(thing, action_time);
  std::string thing_name = thing->get_identifying_string();

  message = this->get_you_or_identifying_string() + " " +
    this->choose_verb("try", "tries") +
    " to take off " + thing_name;
  Service<IMessageLog>::get().add(message);

  switch (deequip_try)
  {
    case ActionResult::Success:
    {
      // Get the body part this item is equipped on.
      WearLocation location;
      this->has_equipped(thing, location);

      if (thing->perform_action_deequipped_by(m_ref, location))
      {
        set_worn(EntityId::Mu(), location);

        std::string wear_desc = get_bodypart_description(location.part, location.number);
        message = this->get_you_or_identifying_string() + " " +
          this->choose_verb("are", "is") + " no longer wearing " + thing_name +
          " on " + this->get_possessive() + " " + wear_desc + ".";
        Service<IMessageLog>::get().add(message);
        return true;
      }
    }
    break;

    case ActionResult::FailureItemNotEquipped:
    {
      message = this->get_you_or_identifying_string() + " " +
        this->choose_verb("are", "is") + " not wearing " + thing_name + ".";
      Service<IMessageLog>::get().add(message);
      return true;
    }
    break;

    default:
      CLOG(WARNING, "Entity") << "Unknown ActionResult " << deequip_try;
      break;
  }

  return false;
}

ActionResult Entity::can_equip(EntityId thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing == m_ref)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's in our inventory.
  if (!this->get_inventory().contains(thing))
  {
    return ActionResult::FailureEntityOutOfReach;
  }

  BodyPart part = thing->is_equippable_on();

  if (part == BodyPart::Count)
  {
    return ActionResult::FailureItemNotEquippable;
  }
  else
  {
    /// @todo Check that entity has free body part(s) to equip item on.
  }

  /// @todo Finish Entity::can_equip code
  return ActionResult::Success;
}

bool Entity::do_equip(EntityId thing, unsigned int& action_time)
{
  std::string message;

  ActionResult equip_try = this->can_equip(thing, action_time);
  std::string thing_name = thing->get_identifying_string();

  switch (equip_try)
  {
    case ActionResult::Success:
    {
      WearLocation location;

      if (thing->perform_action_equipped_by(m_ref, location))
      {
        set_worn(thing, location);

        std::string wear_desc = get_bodypart_description(location.part,
                                                           location.number);
        message = this->get_you_or_identifying_string() + " " +
          this->choose_verb(" are", " is") +
          " now wearing " + thing_name +
          " on " + this->get_possessive() + " " + wear_desc + ".";
        Service<IMessageLog>::get().add(message);
        return true;
      }
    }
    break;

    case ActionResult::FailureSelfReference:
      if (is_player())
      {
        message = "To equip yourself, choose what you want to equip first.";
      }
      else
      {
        message = this->get_you_or_identifying_string() + " " +
          this->choose_verb("try", "tries") +
          " to equip " + this->get_reflexive_pronoun() +
          ", which seriously shouldn't happen.";
        CLOG(WARNING, "Entity") << "NPC tried to equip self!?";
      }
      Service<IMessageLog>::get().add(message);
      break;

    case ActionResult::FailureEntityOutOfReach:
    {
      message = this->get_you_or_identifying_string() + " " +
        this->choose_verb("try", "tries") +
        " to equip " + thing_name + ".";
      Service<IMessageLog>::get().add(message);

      message = thing_name + " is not in " + this->get_possessive() + " inventory.";
      Service<IMessageLog>::get().add(message);
    }
    break;

    case ActionResult::FailureItemNotEquippable:
    {
      message = this->get_you_or_identifying_string() + " " +
        this->choose_verb("try", "tries") +
        " to equip " + thing_name + ".";
      Service<IMessageLog>::get().add(message);

      message = thing_name + " is not an equippable item.";
      Service<IMessageLog>::get().add(message);
    }
    break;

    default:
      CLOG(WARNING, "Entity") << "Unknown ActionResult " << equip_try;
      break;
  }

  return false;
}

void Entity::set_wielded(EntityId thing, unsigned int hand)
{
  if (thing == EntityId::Mu())
  {
    m_wielded_items.erase(hand);
  }
  else
  {
    m_wielded_items[hand] = thing;
  }
}

void Entity::set_worn(EntityId thing, WearLocation location)
{
  if (thing == EntityId::Mu())
  {
    m_equipped_items.erase(location);
  }
  else
  {
    m_equipped_items[location] = thing;
  }
}

bool Entity::can_currently_see()
{
  return get_modified_property<bool>("can_see", false);
}

bool Entity::can_currently_move()
{
  return get_modified_property<bool>("can_move", false);
}

void Entity::set_gender(Gender gender)
{
  m_gender = gender;
}

Gender Entity::get_gender() const
{
  return m_gender;
}

Gender Entity::get_gender_or_you() const
{
  if (is_player())
  {
    return Gender::SecondPerson;
  }
  else
  {
    return m_gender;
  }
}

/// Get the number of a particular body part the DynamicEntity has.
unsigned int Entity::get_bodypart_number(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic<int>("bodypart_body_count");
    case BodyPart::Skin:
      return get_intrinsic<int>("bodypart_skin_count");
    case BodyPart::Head:
      return get_intrinsic<int>("bodypart_head_count");
    case BodyPart::Ear:
      return get_intrinsic<int>("bodypart_ear_count");
    case BodyPart::Eye:
      return get_intrinsic<int>("bodypart_eye_count");
    case BodyPart::Nose:
      return get_intrinsic<int>("bodypart_nose_count");
    case BodyPart::Mouth:
      return get_intrinsic<int>("bodypart_mouth_count");
    case BodyPart::Neck:
      return get_intrinsic<int>("bodypart_neck_count");
    case BodyPart::Chest:
      return get_intrinsic<int>("bodypart_chest_count");
    case BodyPart::Arm:
      return get_intrinsic<int>("bodypart_arm_count");
    case BodyPart::Hand:
      return get_intrinsic<int>("bodypart_hand_count");
    case BodyPart::Leg:
      return get_intrinsic<int>("bodypart_leg_count");
    case BodyPart::Foot:
      return get_intrinsic<int>("bodypart_foot_count");
    case BodyPart::Wing:
      return get_intrinsic<int>("bodypart_wing_count");
    case BodyPart::Tail:
      return get_intrinsic<int>("bodypart_tail_count");
    default:
      return 0;
  }
}

/// Get the appropriate body part name for the DynamicEntity.
std::string Entity::get_bodypart_name(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic<std::string>("bodypart_body_name");
    case BodyPart::Skin:
      return get_intrinsic<std::string>("bodypart_skin_name");
    case BodyPart::Head:
      return get_intrinsic<std::string>("bodypart_head_name");
    case BodyPart::Ear:
      return get_intrinsic<std::string>("bodypart_ear_name");
    case BodyPart::Eye:
      return get_intrinsic<std::string>("bodypart_eye_name");
    case BodyPart::Nose:
      return get_intrinsic<std::string>("bodypart_nose_name");
    case BodyPart::Mouth:
      return get_intrinsic<std::string>("bodypart_mouth_name");
    case BodyPart::Neck:
      return get_intrinsic<std::string>("bodypart_neck_name");
    case BodyPart::Chest:
      return get_intrinsic<std::string>("bodypart_chest_name");
    case BodyPart::Arm:
      return get_intrinsic<std::string>("bodypart_arm_name");
    case BodyPart::Hand:
      return get_intrinsic<std::string>("bodypart_hand_name");
    case BodyPart::Leg:
      return get_intrinsic<std::string>("bodypart_leg_name");
    case BodyPart::Foot:
      return get_intrinsic<std::string>("bodypart_foot_name");
    case BodyPart::Wing:
      return get_intrinsic<std::string>("bodypart_wing_name");
    case BodyPart::Tail:
      return get_intrinsic<std::string>("bodypart_tail_name");
    default:
      return "squeedlyspooch (unknown BodyPart)";
  }
}

/// Get the appropriate body part plural for the DynamicEntity.
std::string Entity::get_bodypart_plural(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic<std::string>("bodypart_body_plural", get_bodypart_name(BodyPart::Body) + "s");
    case BodyPart::Skin:
      return get_intrinsic<std::string>("bodypart_skin_plural", get_bodypart_name(BodyPart::Skin) + "s");
    case BodyPart::Head:
      return get_intrinsic<std::string>("bodypart_head_plural", get_bodypart_name(BodyPart::Head) + "s");
    case BodyPart::Ear:
      return get_intrinsic<std::string>("bodypart_ear_plural", get_bodypart_name(BodyPart::Ear) + "s");
    case BodyPart::Eye:
      return get_intrinsic<std::string>("bodypart_eye_plural", get_bodypart_name(BodyPart::Eye) + "s");
    case BodyPart::Nose:
      return get_intrinsic<std::string>("bodypart_nose_plural", get_bodypart_name(BodyPart::Nose) + "s");
    case BodyPart::Mouth:
      return get_intrinsic<std::string>("bodypart_mouth_plural", get_bodypart_name(BodyPart::Mouth) + "s");
    case BodyPart::Neck:
      return get_intrinsic<std::string>("bodypart_neck_plural", get_bodypart_name(BodyPart::Neck) + "s");
    case BodyPart::Chest:
      return get_intrinsic<std::string>("bodypart_chest_plural", get_bodypart_name(BodyPart::Chest) + "s");
    case BodyPart::Arm:
      return get_intrinsic<std::string>("bodypart_arm_plural", get_bodypart_name(BodyPart::Arm) + "s");
    case BodyPart::Hand:
      return get_intrinsic<std::string>("bodypart_hand_plural", get_bodypart_name(BodyPart::Hand) + "s");
    case BodyPart::Leg:
      return get_intrinsic<std::string>("bodypart_leg_plural", get_bodypart_name(BodyPart::Leg) + "s");
    case BodyPart::Foot:
      return get_intrinsic<std::string>("bodypart_foot_plural", get_bodypart_name(BodyPart::Foot) + "s");
    case BodyPart::Wing:
      return get_intrinsic<std::string>("bodypart_wing_plural", get_bodypart_name(BodyPart::Wing) + "s");
    case BodyPart::Tail:
      return get_intrinsic<std::string>("bodypart_tail_plural", get_bodypart_name(BodyPart::Tail) + "s");
    default:
      return "squeedlyspooches (unknown BodyParts)";
  }
}

bool Entity::is_player() const
{
  return (GAME.get_player() == m_ref);
}

std::string const& Entity::get_type() const
{
  return m_metadata.get_type();
}

std::string const& Entity::get_parent_type() const
{
  return m_metadata.get_intrinsic<std::string>("parent");
}

bool Entity::is_subtype_of(std::string that_type) const
{
  std::string this_type = get_type();
  return GAME.get_things().first_is_subtype_of_second(this_type, that_type);
}

bool Entity::add_modifier(std::string key, EntityId id, unsigned int expiration_ticks)
{
  return m_properties.add_modifier(key, id, expiration_ticks);
}

size_t Entity::remove_modifier(std::string key, EntityId id)
{
  return m_properties.remove_modifier(key, id);
}

unsigned int Entity::get_quantity() const
{
  return get_base_property<unsigned int>("quantity", 1);
}

void Entity::set_quantity(unsigned int quantity)
{
  set_base_property<unsigned int>("quantity", quantity);
}

EntityId Entity::get_id() const
{
  return m_ref;
}

EntityId Entity::get_root_location() const
{
  if (m_location == EntityId::Mu())
  {
    return m_ref;
  }
  else
  {
    auto location = m_location;
    return location->get_root_location();
  }
}

EntityId Entity::get_location() const
{
  return m_location;
}

bool Entity::can_see(EntityId thing)
{
  // Make sure we are able to see at all.
  if (!can_currently_see())
  {
    return false;
  }

  // Are we on a map?  Bail out if we aren't.
  MapId entity_map_id = this->get_map_id();
  MapId thing_map_id = thing->get_map_id();

  if ((entity_map_id == MapFactory::null_map_id) ||
    (thing_map_id == MapFactory::null_map_id) ||
      (entity_map_id != thing_map_id))
  {
    return false;
  }

  auto thing_location = thing->get_maptile();
  if (thing_location == nullptr)
  {
    return false;
  }

  Vec2i thing_coords = thing_location->get_coords();

  return can_see(thing_coords);
}

bool Entity::can_see(Vec2i coords)
{
  // Make sure we are able to see at all.
  if (!can_currently_see())
  {
    return false;
  }

  MapId map_id = get_map_id();

  // Are we on a map?  Bail out if we aren't.
  if (map_id == MapFactory::null_map_id)
  {
    return false;
  }

  // If we aren't on a valid maptile, we can't see anything.
  auto tile = get_maptile();
  if (tile == nullptr)
  {
    return false;
  }

  // If the coordinates are where we are, then yes, we can indeed see the tile, regardless.
  Vec2i tile_coords = tile->get_coords();

  if ((tile_coords.x == coords.x) && (tile_coords.y == coords.y))
  {
    return true;
  }

  Map& game_map = GAME.get_maps().get(map_id);
  auto map_size = game_map.get_size();

  // Check for coords out of bounds. If they're out of bounds, we can't see it.
  if ((coords.x < 0) || (coords.y < 0) || (coords.x >= map_size.x) || (coords.y >= map_size.y))
  {
    return false;
  }

  // Return seen data.
  return m_tiles_currently_seen[game_map.get_index(coords)];
}

void Entity::find_seen_tiles()
{
  //sf::Clock elapsed;

  //elapsed.restart();

  // Are we on a map?  Bail out if we aren't.
  EntityId location = get_location();
  if (location == EntityId::Mu())
  {
    return;
  }

  // Clear the "tile seen" bitset.
  m_tiles_currently_seen.reset();

  // Hang on, can we actually see?
  // If not, bail out.
  if (can_currently_see() == false)
  {
    return;
  }

  /// @todo Handle field-of-view here.
  ///       Field of view for an DynamicEntity can be:
  ///          * NARROW (90 degrees straight ahead)
  ///		   * WIDE (180 degrees in facing direction)
  ///          * FRONTBACK (90 degrees ahead/90 degrees back)
  ///          * FULL (all 360 degrees)
  for (int n = 1; n <= 8; ++n)
  {
    do_recursive_visibility(n);
  }
}

MapMemoryChunk const& Entity::get_memory_at(Vec2i coords) const
{
  static MapMemoryChunk null_memory_chunk{ "???", GAME.get_game_clock() };

  if (this->get_map_id() == MapFactory::null_map_id)
  {
    return null_memory_chunk;
  }

  Map& game_map = GAME.get_maps().get(this->get_map_id());
  return m_map_memory[game_map.get_index(coords)];
}

bool Entity::move_into(EntityId new_location)
{
  MapId old_map_id = this->get_map_id();
  EntityId old_location = m_location;

  if (new_location == old_location)
  {
    // We're already there!
    return true;
  }

  ActionResult can_contain = new_location->can_contain(m_ref);

  switch (can_contain)
  {
    case ActionResult::Success:
      if (new_location->get_inventory().add(m_ref))
      {
        // Try to lock our old location.
        if (m_location != EntityId::Mu())
        {
          m_location->get_inventory().remove(m_ref);
        }

        // Set the location to the new location.
        m_location = new_location;

        MapId new_map_id = this->get_map_id();
        if (old_map_id != new_map_id)
        {
          if (old_map_id != MapFactory::null_map_id)
          {
            /// @todo Save old map memory.
          }
          m_map_memory.clear();
          m_tiles_currently_seen.clear();
          if (new_map_id != MapFactory::null_map_id)
          {
            Map& new_map = GAME.get_maps().get(new_map_id);
            Vec2i new_map_size = new_map.get_size();
            m_map_memory.resize(new_map_size.x * new_map_size.y);
            m_tiles_currently_seen.resize(new_map_size.x * new_map_size.y);
            /// @todo Load new map memory if it exists somewhere.
          }
        }
        this->find_seen_tiles();
        notifyObservers(Event::Updated);
        return true;
      } // end if (add to new inventory was successful)

      break;
    default:
      break;
  } // end switch (ActionResult)

  return false;
}

Inventory& Entity::get_inventory()
{
  return m_inventory;
}

bool Entity::is_inside_another_thing() const
{
  EntityId location = m_location;
  if (location == EntityId::Mu())
  {
    // Entity is a part of the MapTile such as the floor.
    return false;
  }

  EntityId location2 = location->get_location();
  if (location2 == EntityId::Mu())
  {
    // Entity is directly on the floor.
    return false;
  }
  return true;
}

MapTile* Entity::get_maptile() const
{
  EntityId location = m_location;

  if (location == EntityId::Mu())
  {
    return _get_maptile();
  }
  else
  {
    return location->get_maptile();
  }
}

MapId Entity::get_map_id() const
{
  EntityId location = m_location;

  if (location == EntityId::Mu())
  {
    MapTile* maptile = _get_maptile();
    if (maptile != nullptr)
    {
      return maptile->get_map_id();
    }
    else
    {
      return static_cast<MapId>(0);
    }
  }
  else
  {
    return location->get_map_id();
  }
}

std::string Entity::get_display_adjectives() const
{
  std::string adjectives;

  if (is_subtype_of("DynamicEntity") && get_modified_property<int>("hp") <= 0)
  {
    adjectives += "dead ";
  }

  /// @todo Implement more adjectives.
  return adjectives;
}

std::string Entity::get_display_name() const
{
  return m_metadata.get_intrinsic<std::string>("name");
}

std::string Entity::get_display_plural() const
{
  return m_metadata.get_intrinsic<std::string>("plural");
}

std::string Entity::get_proper_name() const
{
  return get_modified_property<std::string>("proper_name");
}

void Entity::set_proper_name(std::string name)
{
  set_base_property<std::string>("proper_name", name);
}

std::string Entity::get_you_or_identifying_string(ArticleChoice articles) const
{
  std::string str;

  if (is_player())
  {
    if (get_modified_property<int>("hp") > 0)
    {
      str += "you";
    }
    else
    {
      str += "your corpse";
    }
  }
  else
  {
    str += get_identifying_string(articles);
  }

  return str;
}

std::string Entity::get_self_or_identifying_string(EntityId other, ArticleChoice articles) const
{
  if (other == get_id())
  {
    return get_reflexive_pronoun();
  }

  return get_identifying_string(articles);
}

std::string Entity::get_identifying_string(ArticleChoice articles,
                                            UsePossessives possessives) const
{
  auto& config = Service<IConfigSettings>::get();

  EntityId location = this->get_location();
  unsigned int quantity = this->get_quantity();

  std::string name;

  bool owned;

  std::string debug_prefix;
  if (config.get<bool>("debug_show_thing_ids") == true)
  {
    debug_prefix = "(#" + static_cast<std::string>(get_id()) + ") ";
  }
  
  std::string article;
  std::string adjectives;
  std::string noun;
  std::string suffix;

  owned = location->is_subtype_of("DynamicEntity");

  if (quantity == 1)
  {
    noun = get_display_name();

    if (owned && (possessives == UsePossessives::Yes))
    {
      article = location->get_possessive() + " ";
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        article = tr("ARTICLE_DEFINITE") + " ";
      }
      else
      {
        article = getIndefArt(noun) + " ";
      }
    }

    if (get_proper_name().empty() == false)
    {
      suffix = " named " + get_proper_name();
    }
  }
  else
  {
    noun = get_display_plural();

    if (owned && (possessives == UsePossessives::Yes))
    {
      article = location->get_possessive() + " ";
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        article = "the ";
      }

      article += std::to_string(get_quantity()) + " ";
    }
  }

  adjectives = get_display_adjectives();

  name = debug_prefix + article + adjectives + noun + suffix;

  return name;
}

bool Entity::is_third_person()
{
  return (GAME.get_player() == m_ref) || (get_base_property<unsigned int>("quantity") > 1);
}

std::string const& Entity::choose_verb(std::string const& verb12,
                                        std::string const& verb3)
{
  if ((GAME.get_player() == m_ref) || (get_base_property<unsigned int>("quantity") > 1))
  {
    return verb12;
  }
  else
  {
    return verb3;
  }
}

int Entity::get_mass()
{
  return get_modified_property<int>("physical_mass") * get_base_property<unsigned int>("quantity");
}

std::string const& Entity::get_subject_pronoun() const
{
  return getSubjPro(get_gender_or_you());
}

std::string const& Entity::get_object_pronoun() const
{
  return getObjPro(get_gender_or_you());
}

std::string const& Entity::get_reflexive_pronoun() const
{
  return getRefPro(get_gender_or_you());
}

std::string const& Entity::get_possessive_adjective() const
{
  return getPossAdj(get_gender_or_you());
}

std::string const& Entity::get_possessive_pronoun() const
{
  return getPossPro(get_gender_or_you());
}

std::string Entity::get_possessive()
{
  if (GAME.get_player() == m_ref)
  {
    return "your";
  }
  else
  {
    return get_identifying_string(ArticleChoice::Definite,
                                  UsePossessives::No) + "'s";
  }
}

Vec2u Entity::get_tile_sheet_coords(int frame)
{
  /// Get tile coordinates on the sheet.
  Vec2u start_coords = m_metadata.get_tile_coords();

  /// Call the Lua function to get the offset (tile to choose).
  Vec2u offset = call_lua_function<Vec2u>("get_tile_offset", { frame });

  /// Add them to get the resulting coordinates.
  Vec2u tile_coords = start_coords + offset;

  return tile_coords;
}

bool Entity::is_opaque()
{
  return
    (get_modified_property<int>("opacity_red") >= 255) &&
    (get_modified_property<int>("opacity_green") >= 255) &&
    (get_modified_property<int>("opacity_blue") >= 255);
}

void Entity::light_up_surroundings()
{
  EntityId location = get_location();

  if (get_intrinsic<int>("inventory_size") != 0)
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in an entity's inventory.

    bool opaque = location->is_opaque();
    bool is_entity = this->is_subtype_of("DynamicEntity");

    /*CLOG(DEBUG, "Entity") << "light_up_surroundings - this->type = " << this->get_type() <<
      ", location->type = " << location->get_type() <<
      ", location->opaque = " << opaque <<
      ", this->is_subtype_of(\"DynamicEntity\") = " << is_entity;*/

    //if (!is_opaque() || is_wielding(light) || has_equipped(light))
    if (!opaque || is_entity)
    {
      auto& inventory = get_inventory();
      for (auto iter = inventory.begin();
           iter != inventory.end();
           ++iter)
      {
        EntityId thing = iter->second;
        thing->light_up_surroundings();
      }
    }
  }

  // Use visitor pattern.
  if ((location != EntityId::Mu()) && this->get_modified_property<bool>("lit"))
  {
    location->be_lit_by(this->get_id());
  }
}

void Entity::be_lit_by(EntityId light)
{
  auto result = call_lua_function<ActionResult, EntityId>("on_lit_by", { light }, ActionResult::Success);
  if (result == ActionResult::Success) notifyObservers(Event::Updated);

  EntityId location = get_location();

  if (get_location() == EntityId::Mu())
  {
    GAME.get_maps().get(get_map_id()).add_light(light);
  }
  else
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in the player's inventory.

    bool opaque = this->is_opaque();
    bool is_entity = this->is_subtype_of("DynamicEntity");

    //if (!is_opaque() || is_wielding(light) || has_equipped(light))
    if (!opaque || is_entity)
    {
      location->be_lit_by(light);
    }
  }
}

void Entity::spill()
{
  Inventory& inventory = get_inventory();
  std::string message;
  bool success = false;

  // Step through all contents of this Entity.
  for (auto iter = inventory.begin();
       iter != inventory.end();
       ++iter)
  {
    EntityId thing = iter->second;
    if (m_location != EntityId::Mu())
    {
      ActionResult can_contain = m_location->can_contain(thing);

      switch (can_contain)
      {
        case ActionResult::Success:

          // Try to move this into the Entity's location.
          success = thing->move_into(m_location);
          if (success)
          {
            auto container_string = this->get_identifying_string();
            auto thing_string = thing->get_identifying_string();
            message = thing_string + this->choose_verb(" tumble", " tumbles") + " out of " + container_string + ".";
            Service<IMessageLog>::get().add(message);
          }
          else
          {
            // We couldn't move it, so just destroy it.
            auto container_string = this->get_identifying_string();
            auto thing_string = thing->get_identifying_string();
            message = thing_string + this->choose_verb(" vanish", " vanishes") + " in a puff of logic.";
            Service<IMessageLog>::get().add(message);
            thing->destroy();
          }

          notifyObservers(Event::Updated);

          break;

        case ActionResult::FailureInventoryFull:
          /// @todo Handle the situation where the Entity's container can't hold the Entity.
          break;

        default:
          break;
      } // end switch (can_contain)
    } // end if (container location is not Mu)
    else
    {
      // Container's location is Mu, so just destroy it without a message.
      thing->destroy();
    }
  } // end for (contents of Entity)
}

void Entity::destroy()
{
  auto old_location = m_location;

  if (get_intrinsic<int>("inventory_size") != 0)
  {
    // Spill the contents of this Entity into the Entity's location.
    spill();
  }

  if (old_location != EntityId::Mu())
  {
    old_location->get_inventory().remove(m_ref);
  }

  notifyObservers(Event::Updated);
}

std::string Entity::get_bodypart_description(BodyPart part,
                                              unsigned int number)
{
  unsigned int total_number = this->get_bodypart_number(part);
  std::string part_name = this->get_bodypart_name(part);
  std::string result;

  ASSERT_CONDITION(number < total_number);
  switch (total_number)
  {
    case 0: // none of them!?  shouldn't occur!
      result = "non-existent " + part_name;
      CLOG(WARNING, "Entity") << "Request for description of " << result << "!?";
      break;

    case 1: // only one of them
      result = part_name;
      break;

    case 2: // assume a right and left one.
      switch (number)
      {
        case 0:
          result = "right " + part_name;
          break;
        case 1:
          result = "left " + part_name;
          break;
        default:
          break;
      }
      break;

    case 3: // assume right, center, and left.
      switch (number)
      {
        case 0:
          result = "right " + part_name;
          break;
        case 1:
          result = "center " + part_name;
          break;
        case 2:
          result = "left " + part_name;
          break;
        default:
          break;
      }
      break;

    case 4: // Legs/feet assume front/rear, others assume upper/lower.
      if ((part == BodyPart::Leg) || (part == BodyPart::Foot))
      {
        switch (number)
        {
          case 0:
            result = "front right " + part_name;
            break;
          case 1:
            result = "front left " + part_name;
            break;
          case 2:
            result = "rear right " + part_name;
            break;
          case 3:
            result = "rear left " + part_name;
            break;
          default:
            break;
        }
      }
      else
      {
        switch (number)
        {
          case 0:
            result = "upper right " + part_name;
            break;
          case 1:
            result = "upper left " + part_name;
            break;
          case 2:
            result = "lower right " + part_name;
            break;
          case 3:
            result = "lower left " + part_name;
            break;
          default:
            break;
        }
      }
      break;

    case 6: // Legs/feet assume front/middle/rear, others upper/middle/lower.
      if ((part == BodyPart::Leg) || (part == BodyPart::Foot))
      {
        switch (number)
        {
          case 0:
            result = "front right " + part_name;
            break;
          case 1:
            result = "front left " + part_name;
            break;
          case 2:
            result = "middle right " + part_name;
            break;
          case 3:
            result = "middle left " + part_name;
            break;
          case 4:
            result = "rear right " + part_name;
            break;
          case 5:
            result = "rear left " + part_name;
            break;
          default:
            break;
        }
      }
      else
      {
        switch (number)
        {
          case 0:
            result = "upper right " + part_name;
            break;
          case 1:
            result = "upper left " + part_name;
            break;
          case 2:
            result = "middle right " + part_name;
            break;
          case 3:
            result = "middle left " + part_name;
            break;
          case 4:
            result = "lower right " + part_name;
            break;
          case 5:
            result = "lower left " + part_name;
            break;
          default:
            break;
        }
      }
      break;

    default:
      break;
  }

  // Anything else and we just return the ordinal name.
  if (result.empty())
  {
    result = Ordinal::get(number) + " " + part_name;
  }

  return result;
}

bool Entity::can_have_action_done_by(EntityId thing, Action& action)
{
  return call_lua_function<bool>("can_have_action_" + action.get_type() + "_done_by", { thing }, false);
}

bool Entity::is_miscible_with(EntityId thing)
{
  return call_lua_function<bool>("is_miscible_with", { thing }, false);
}

BodyPart Entity::is_equippable_on() const
{
  return BodyPart::Count;
}

bool Entity::process()
{
  // Get a copy of the Entity's inventory.
  // This is because things can be deleted/removed from the inventory
  // over the course of processing them, and this could invalidate the
  // iterator.
  Inventory temp_inventory{ m_inventory };

  // Process inventory.
  for (auto iter = temp_inventory.begin();
       iter != temp_inventory.end();
       ++iter)
  {
    EntityId thing = iter->second;
    /* bool dead = */ thing->process();
  }

  // Process self last.
  return _process_self();
}

ActionResult Entity::perform_action_died()
{
  ActionResult result = call_lua_function<ActionResult>("perform_action_died", {}, ActionResult::Success);
  return result;
}

void Entity::perform_action_collided_with(EntityId actor)
{
  /* ActionResult result = */ call_lua_function<ActionResult, EntityId>("perform_action_collided_with", { actor }, ActionResult::Success);
  return;
}

void Entity::perform_action_collided_with_wall(Direction d, std::string tile_type)
{
  /// @todo Implement me; right now there's no way to pass one enum and one string to a Lua function.
  return;
}

ActionResult Entity::be_object_of(Action& action, EntityId subject)
{
  ActionResult result = call_lua_function<ActionResult, EntityId>("be_object_of_action_" + action.get_type(), { subject }, ActionResult::Success);
  return result;
}

ActionResult Entity::be_object_of(Action & action, EntityId subject, EntityId target)
{
  ActionResult result = call_lua_function<ActionResult, EntityId>("be_object_of_action_" + action.get_type(), { subject, target }, ActionResult::Success);
  return result;
}

ActionResult Entity::be_object_of(Action & action, EntityId subject, Direction direction)
{
  ActionResult result = call_lua_function<ActionResult>("be_object_of_action_" + action.get_type(), { subject, NULL, direction.x(), direction.y(), direction.z() }, ActionResult::Success);
  return result;
}

ActionResult Entity::perform_action_hurt_by(EntityId subject)
{
  ActionResult result = call_lua_function<ActionResult, EntityId>("be_object_of_action_hurt", { subject }, ActionResult::Success);
  return result;
}

ActionResult Entity::perform_action_attacked_by(EntityId subject, EntityId target)
{
  ActionResult result = call_lua_function<ActionResult, EntityId>("be_object_of_action_attack", { subject, target }, ActionResult::Success);
  return result;
}

bool Entity::perform_action_deequipped_by(EntityId actor, WearLocation& location)
{
  if (this->get_modified_property<bool>("bound"))
  {
    std::string message;
    message = actor->get_identifying_string() + " cannot take off " + this->get_identifying_string() +
      "; it is magically bound to " +
      actor->get_possessive_adjective() + " " +
      actor->get_bodypart_description(location.part,
                                      location.number) + "!";
    Service<IMessageLog>::get().add(message);
    return false;
  }
  else
  {
    ActionResult result = call_lua_function<ActionResult, EntityId>("perform_action_deequipped_by", { actor }, ActionResult::Success);
    return was_successful(result);
  }
}

bool Entity::perform_action_equipped_by(EntityId actor, WearLocation& location)
{
  ActionResult result = call_lua_function<ActionResult, EntityId>("perform_action_equipped_by", { actor }, ActionResult::Success);
  bool subclass_result = was_successful(result);

  return subclass_result;
}

bool Entity::can_merge_with(EntityId other) const
{
  // Entities with different types can't merge (obviously).
  if (other->get_type() != get_type())
  {
    return false;
  }

  // Entities with inventories can never merge.
  if ((get_intrinsic<int>("inventory_size") != 0) ||
    (other->get_intrinsic<int>("inventory_size") != 0))
  {
    return false;
  }

  // If the things have the exact same properties, merge is okay.
  /// @todo Handle default properties. Right now, if a property was
  ///       queried on a Entity and pulls the default, but it was NOT queried
  ///       on the second thing, the property dictionaries will NOT match.
  ///       I have not yet found a good solution to this problem.
  auto& our_properties = this->m_properties;
  auto& other_properties = other->m_properties;

  if ((this->m_properties) == (other->m_properties))
  {
    return true;
  }

  return false;
}

ActionResult Entity::can_contain(EntityId thing)
{
  unsigned int inventory_size = get_intrinsic<unsigned int>("inventory_size");
  if (inventory_size == 0)
  {
    return ActionResult::FailureTargetNotAContainer;
  }
  else if (get_inventory().count() >= inventory_size)
  {
    return ActionResult::FailureInventoryFull;
  }
  else
  {
    return call_lua_function<ActionResult, EntityId>("can_contain", { thing }, ActionResult::Success);
  }
}

void Entity::set_location(EntityId target)
{
  m_location = target;
}

// *** PROTECTED METHODS ******************************************************

bool Entity::_process_self()
{
  int counter_busy = get_base_property<int>("counter_busy");

  // Is this an entity that is now dead?
  if (is_subtype_of("DynamicEntity") && (get_modified_property<int>("hp") <= 0))
  {
    // Did the entity JUST die?
    if (get_modified_property<bool>("dead") != true)
    {
      // Perform the "die" action.
      // (This sets the "dead" property and clears out any pending actions.)
      if (this->do_die() == true)
      {
        /// @todo Handle player death by transitioning to game end state.
      }
    }
  }
  // Otherwise if this entity is busy...
  else if (counter_busy > 0)
  {
    // Decrement busy counter.
    add_to_base_property<int>("counter_busy", -1);
  }
  // Otherwise if actions are pending...
  else if (!m_pending_actions.empty())
  {
    // Process the front action.
    std::unique_ptr<Action>& action = m_pending_actions.front();
    bool action_done = action->process(get_id(), {});
    if (action_done)
    {
      CLOG(TRACE, "Entity") << "Entity " <<
        get_id() << "( " <<
        get_type() << "): Action " <<
        action->get_type() << " is done, popping";

      m_pending_actions.pop_front();
    }

    /// @todo This needs to be changed so it only is called if the Action
    ///       materially affected the thing in some way. Two ways to do this
    ///       that I can see:
    ///         1) The Action calls notifyObservers. Requires the Action
    ///            to have access to that method; right now it doesn't.
    ///         2) The Action returns some sort of indication that the Entity
    ///            was modified as a result. This could be done by changing
    ///            the return type from a bool to a struct of some sort.
    notifyObservers(Event::Updated);

  } // end if (actions pending)
  // Otherwise if there are no other pending actions...
  else
  {
    // If entity is not the player, call the Lua process function on this Entity.
    /// @todo Should this be called regardless of whether actions are pending
    ///       or not? In other words, should a Entity be able to "change its mind"
    ///       and clear out any pending actions?
    if (!is_player())
    {
      ActionResult result = call_lua_function<ActionResult>("process");
    }
  }

  return true;
}

MapMemory& Entity::get_map_memory()
{
  return m_map_memory;
}

void Entity::do_recursive_visibility(int octant,
                                    int depth,
                                    float slope_A,
                                    float slope_B)
{
  Vec2i new_coords;
  //int x = 0;
  //int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (is_inside_another_thing())
  {
    return;
  }

  MapTile* tile = get_maptile();
  Vec2i tile_coords = tile->get_coords();
  Map& game_map = GAME.get_maps().get(get_map_id());

  static const int mv = 128;
  static constexpr int mw = (mv * mv);

  std::function< bool(Vec2f, Vec2f, float) > loop_condition;
  Direction dir;
  std::function< float(Vec2f, Vec2f) > recurse_slope;
  std::function< float(Vec2f, Vec2f) > loop_slope;

  switch (octant)
  {
    case 1:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Northwest.half(), b); };
      break;

    case 2:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_slope(a + Direction::Northeast.half(), b); };
      break;

    case 3:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
      break;

    case 4:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      break;

    case 5:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Southeast.half(), b); };
      break;

    case 6:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_slope(a + Direction::Southwest.half(), b); };
      break;

    case 7:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
      break;

    case 8:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](Vec2f a, Vec2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](Vec2f a, Vec2f b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      break;

    default:
      MAJOR_ERROR("Octant passed to do_recursive_visibility was %d (not 1 to 8)!", octant);
      break;
  }

  while (loop_condition(to_v2f(new_coords), to_v2f(tile_coords), slope_B))
  {
    if (calc_vis_distance(new_coords, tile_coords) <= mw)
    {
      if (game_map.tile_is_opaque(new_coords))
      {
        if (!game_map.tile_is_opaque(new_coords + (Vec2i)dir))
        {
          do_recursive_visibility(octant, depth + 1, slope_A, recurse_slope(to_v2f(new_coords), to_v2f(tile_coords)));
        }
      }
      else
      {
        if (game_map.tile_is_opaque(new_coords + (Vec2i)dir))
        {
          slope_A = loop_slope(to_v2f(new_coords), to_v2f(tile_coords));
        }
      }
      m_tiles_currently_seen[game_map.get_index(new_coords)] = true;

      MapMemoryChunk new_memory{ game_map.get_tile(new_coords).get_tile_type(),
                                 GAME.get_game_clock() };
      m_map_memory[game_map.get_index(new_coords)] = new_memory;
    }
    new_coords -= (Vec2i)dir;
  }
  new_coords += (Vec2i)dir;

  if ((depth < mv) && (!game_map.get_tile(new_coords).is_opaque()))
  {
    do_recursive_visibility(octant, depth + 1, slope_A, slope_B);
  }
}

MapTile* Entity::_get_maptile() const
{
  return m_map_tile;
}