#include "stdafx.h"

#include "entity/Entity.h"

#include "actions/Action.h"
#include "actions/ActionDie.h"
#include "AssertHelper.h"
#include "entity/EntityPool.h"
#include "game/App.h"
#include "game/GameState.h"
#include "inventory/Inventory.h"
#include "map/Map.h"
#include "maptile/MapTile.h"
#include "metadata/Metadata.h"
#include "Service.h"
#include "services/IConfigSettings.h"
#include "services/IStringDictionary.h"
#include "services/MessageLog.h"
#include "types/Direction.h"
#include "types/Gender.h"
#include "types/IntegerRange.h"
#include "utilities/MathUtils.h"
#include "utilities/Ordinal.h"
#include "utilities/StringTransforms.h"


// Static member initialization.
sf::Color const Entity::wall_outline_color_ = sf::Color(255, 255, 255, 64);

Metadata const & Entity::get_metadata() const
{
  return m_metadata;
}

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
  m_pending_involuntary_actions{ ActionQueue() },
  m_pending_voluntary_actions{ ActionQueue() },
  m_wielded_items{ BodyLocationMap() },
  m_equipped_items{ BodyLocationMap() }
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
  m_pending_involuntary_actions{ ActionQueue() },
  m_pending_voluntary_actions{ ActionQueue() },
  m_wielded_items{ BodyLocationMap() },
  m_equipped_items{ BodyLocationMap() }
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
  m_pending_involuntary_actions{ ActionQueue() },     // don't copy
  m_pending_voluntary_actions{ ActionQueue() },     // don't copy
  m_wielded_items{ BodyLocationMap() },       // don't copy
  m_equipped_items{ BodyLocationMap() }        // don't copy
{
  initialize();
}

void Entity::initialize()
{
  /// Get our maximum HP. (The Lua script will automatically pick it from a range.)
  auto max_hp = m_metadata.get_intrinsic("maxhp");
  set_base_property("maxhp", max_hp);

  /// Also set our HP to that value.
  set_base_property("hp", max_hp);

  //notifyObservers(Event::Updated);
}

Entity::~Entity()
{
}

void Entity::queue_action(std::unique_ptr<Actions::Action> action)
{
  CLOG(TRACE, "Entity") << "Entity " <<
    get_id() << "( " <<
    get_type() << "): Queuing Action " <<
    action->get_type();

  m_pending_voluntary_actions.push_back(std::move(action));
}

void Entity::queue_action(Actions::Action * p_action)
{
  std::unique_ptr<Actions::Action> action(p_action);
  queue_action(std::move(action));
}

void Entity::queue_involuntary_action(std::unique_ptr<Actions::Action> action)
{
  CLOG(TRACE, "Entity") << "Entity " <<
    get_id() << "( " <<
    get_type() << "): Queuing Involuntary Action " <<
    action->get_type();

  m_pending_involuntary_actions.push_front(std::move(action));
}

void Entity::queue_involuntary_action(Actions::Action * p_action)
{
  std::unique_ptr<Actions::Action> action(p_action);
  queue_involuntary_action(std::move(action));
}

bool Entity::action_is_pending() const
{
  return !voluntary_action_is_pending() || !involuntary_action_is_pending();
}

bool Entity::voluntary_action_is_pending() const
{
  return !(m_pending_voluntary_actions.empty());
}

bool Entity::involuntary_action_is_pending() const
{
  return !(m_pending_involuntary_actions.empty());
}

bool Entity::action_is_in_progress()
{
  return (get_base_property("counter_busy").as<uint32_t>() > 0);
}

void Entity::clear_pending_actions()
{
  clear_pending_voluntary_actions();
  clear_pending_involuntary_actions();
}

void Entity::clear_pending_voluntary_actions()
{
  m_pending_voluntary_actions.clear();
}

void Entity::clear_pending_involuntary_actions()
{
  m_pending_involuntary_actions.clear();
}

EntityId Entity::get_wielding_in(BodyLocation& location)
{
  if (m_wielded_items.count(location) == 0)
  {
    return EntityId::Mu();
  }
  else
  {
    return m_wielded_items[location];
  }
}

bool Entity::is_wielding(EntityId entity)
{
  BodyLocation dummy;
  return is_wielding(entity, dummy);
}

bool Entity::is_wielding(EntityId entity, BodyLocation& location)
{
  if (entity == EntityId::Mu())
  {
    return false;
  }
  auto found_item =
    std::find_if(m_wielded_items.cbegin(),
                 m_wielded_items.cend(),
                 [&](BodyLocationPair const& p)
  { return p.second == entity; });

  if (found_item == m_wielded_items.cend())
  {
    return false;
  }
  else
  {
    location = found_item->first;
    return true;
  }
}

bool Entity::is_wearing(EntityId entity)
{
  BodyLocation dummy;
  return is_wearing(entity, dummy);
}

bool Entity::is_wearing(EntityId entity, BodyLocation& location)
{
  if (entity == EntityId::Mu())
  {
    return false;
  }
  auto found_item =
    std::find_if(m_equipped_items.cbegin(),
                 m_equipped_items.cend(),
                 [&](BodyLocationPair const& p)
  { return p.second == entity; });

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

bool Entity::can_reach(EntityId entity)
{
  // Check if it is our location.
  auto our_location = getLocation();
  if (our_location == entity)
  {
    return true;
  }

  // Check if it's at our location.
  auto thing_location = entity->getLocation();
  if (our_location == thing_location)
  {
    return true;
  }

  // Check if it's in our inventory.
  if (this->get_inventory().contains(entity))
  {
    return true;
  }

  return false;
}

bool Entity::is_adjacent_to(EntityId entity)
{
  // Get the coordinates we are at.
  MapTile* our_maptile = get_maptile();
  MapTile* thing_maptile = entity->get_maptile();
  if ((our_maptile == nullptr) || (thing_maptile == nullptr))
  {
    return false;
  }

  IntVec2 const& our_coords = our_maptile->get_coords();
  IntVec2 const& thing_coords = thing_maptile->get_coords();

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
        message = StringTransforms::maketr(get_id(), 0, "YOU_DIE");
        Service<IMessageLog>::get().add(message);
      }
      else
      {
        bool living = get_modified_property("living").as<bool>();
        message = StringTransforms::maketr(get_id(), 0, living ? "YOU_ARE_KILLED" : "YOU_ARE_DESTROYED");
        Service<IMessageLog>::get().add(message);
      }

      // Set the property saying the entity is dead.
      set_base_property("dead", Property::from(true));

      // Clear any pending actions.
      clear_pending_actions();

      //notifyObservers(Event::Updated);
      return true;
    case ActionResult::Failure:
    default:
      message = StringTransforms::maketr(get_id(), 0, "YOU_MANAGE_TO_AVOID_DYING");
      Service<IMessageLog>::get().add(message);
      return false;
  }
}

void Entity::set_wielded(EntityId entity, BodyLocation location)
{
  if (entity == EntityId::Mu())
  {
    m_wielded_items.erase(location);
  }
  else
  {
    m_wielded_items[location] = entity;
  }
}

void Entity::set_worn(EntityId entity, BodyLocation location)
{
  if (entity == EntityId::Mu())
  {
    m_equipped_items.erase(location);
  }
  else
  {
    m_equipped_items[location] = entity;
  }
}

bool Entity::can_currently_see()
{
  return get_modified_property("can_see", Property::from(false)).as<bool>();
}

bool Entity::can_currently_move()
{
  return get_modified_property("can_move", Property::from(false)).as<bool>();
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
    if (get_quantity() > 1)
    {
      return Gender::Plural;
    }
    else
    {
      return m_gender;
    }
  }
}

/// Get the number of a particular body part the DynamicEntity has.
Property Entity::get_bodypart_number(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic("bodypart_body_count");
    case BodyPart::Skin:
      return get_intrinsic("bodypart_skin_count");
    case BodyPart::Head:
      return get_intrinsic("bodypart_head_count");
    case BodyPart::Ear:
      return get_intrinsic("bodypart_ear_count");
    case BodyPart::Eye:
      return get_intrinsic("bodypart_eye_count");
    case BodyPart::Nose:
      return get_intrinsic("bodypart_nose_count");
    case BodyPart::Mouth:
      return get_intrinsic("bodypart_mouth_count");
    case BodyPart::Neck:
      return get_intrinsic("bodypart_neck_count");
    case BodyPart::Chest:
      return get_intrinsic("bodypart_chest_count");
    case BodyPart::Arm:
      return get_intrinsic("bodypart_arm_count");
    case BodyPart::Hand:
      return get_intrinsic("bodypart_hand_count");
    case BodyPart::Leg:
      return get_intrinsic("bodypart_leg_count");
    case BodyPart::Foot:
      return get_intrinsic("bodypart_foot_count");
    case BodyPart::Wing:
      return get_intrinsic("bodypart_wing_count");
    case BodyPart::Tail:
      return get_intrinsic("bodypart_tail_count");
    default:
      return Property::from(0);
  }
}

/// Get the appropriate body part name for the DynamicEntity.
/// @todo Figure out how to cleanly localize this.
Property Entity::get_bodypart_name(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic("bodypart_body_name");
    case BodyPart::Skin:
      return get_intrinsic("bodypart_skin_name");
    case BodyPart::Head:
      return get_intrinsic("bodypart_head_name");
    case BodyPart::Ear:
      return get_intrinsic("bodypart_ear_name");
    case BodyPart::Eye:
      return get_intrinsic("bodypart_eye_name");
    case BodyPart::Nose:
      return get_intrinsic("bodypart_nose_name");
    case BodyPart::Mouth:
      return get_intrinsic("bodypart_mouth_name");
    case BodyPart::Neck:
      return get_intrinsic("bodypart_neck_name");
    case BodyPart::Chest:
      return get_intrinsic("bodypart_chest_name");
    case BodyPart::Arm:
      return get_intrinsic("bodypart_arm_name");
    case BodyPart::Hand:
      return get_intrinsic("bodypart_hand_name");
    case BodyPart::Leg:
      return get_intrinsic("bodypart_leg_name");
    case BodyPart::Foot:
      return get_intrinsic("bodypart_foot_name");
    case BodyPart::Wing:
      return get_intrinsic("bodypart_wing_name");
    case BodyPart::Tail:
      return get_intrinsic("bodypart_tail_name");
    default:
      return Property::from("squeedlyspooch (unknown BodyPart)");
  }
}

/// Get the appropriate body part plural for the DynamicEntity.
/// @todo Figure out how to cleanly localize this.
Property Entity::get_bodypart_plural(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic("bodypart_body_plural", get_bodypart_name(BodyPart::Body) + Property::from("s"));
    case BodyPart::Skin:
      return get_intrinsic("bodypart_skin_plural", get_bodypart_name(BodyPart::Skin) + Property::from("s"));
    case BodyPart::Head:
      return get_intrinsic("bodypart_head_plural", get_bodypart_name(BodyPart::Head) + Property::from("s"));
    case BodyPart::Ear:
      return get_intrinsic("bodypart_ear_plural", get_bodypart_name(BodyPart::Ear) + Property::from("s"));
    case BodyPart::Eye:
      return get_intrinsic("bodypart_eye_plural", get_bodypart_name(BodyPart::Eye) + Property::from("s"));
    case BodyPart::Nose:
      return get_intrinsic("bodypart_nose_plural", get_bodypart_name(BodyPart::Nose) + Property::from("s"));
    case BodyPart::Mouth:
      return get_intrinsic("bodypart_mouth_plural", get_bodypart_name(BodyPart::Mouth) + Property::from("s"));
    case BodyPart::Neck:
      return get_intrinsic("bodypart_neck_plural", get_bodypart_name(BodyPart::Neck) + Property::from("s"));
    case BodyPart::Chest:
      return get_intrinsic("bodypart_chest_plural", get_bodypart_name(BodyPart::Chest) + Property::from("s"));
    case BodyPart::Arm:
      return get_intrinsic("bodypart_arm_plural", get_bodypart_name(BodyPart::Arm) + Property::from("s"));
    case BodyPart::Hand:
      return get_intrinsic("bodypart_hand_plural", get_bodypart_name(BodyPart::Hand) + Property::from("s"));
    case BodyPart::Leg:
      return get_intrinsic("bodypart_leg_plural", get_bodypart_name(BodyPart::Leg) + Property::from("s"));
    case BodyPart::Foot:
      return get_intrinsic("bodypart_foot_plural", get_bodypart_name(BodyPart::Foot) + Property::from("s"));
    case BodyPart::Wing:
      return get_intrinsic("bodypart_wing_plural", get_bodypart_name(BodyPart::Wing) + Property::from("s"));
    case BodyPart::Tail:
      return get_intrinsic("bodypart_tail_plural", get_bodypart_name(BodyPart::Tail) + Property::from("s"));
    default:
      return Property::from("squeedlyspooches (unknown BodyParts)");
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

std::string Entity::get_parent_type() const
{
  return m_metadata.get_intrinsic("parent").as<std::string>();
}

bool Entity::is_subtype_of(std::string that_type) const
{
  std::string this_type = get_type();
  return GAME.get_entities().first_is_subtype_of_second(this_type, that_type);
}

Property Entity::get_intrinsic(std::string key, Property default_value) const
{
  return m_metadata.get_intrinsic(key, default_value);
}

Property Entity::get_intrinsic(std::string key) const
{
  return get_intrinsic(key, Property());
}

Property Entity::get_base_property(std::string key, Property default_value) const
{
  if (m_properties.contains(key))
  {
    return m_properties.get(key);
  }
  else
  {
    Property value = m_metadata.get_intrinsic(key, default_value);
    m_properties.set(key, Property(value));
    return value;
  }
}

Property Entity::get_base_property(std::string key) const
{
  return get_base_property(key, Property());
}

/// Sets a base property of this Entity.
/// If the base property is not found, it is created.
///
/// @param key    Key of the property to set.
/// @param value  Value to set the property to.
/// @return Boolean indicating whether the property previously existed.
bool Entity::set_base_property(std::string key, Property value)
{
  bool existed = m_properties.contains(key);
  m_properties.set(key, value);

  return existed;
}

/// Adds to a base property of this Entity.
/// If the base property is not found, it is created.
/// @param key    Key of the property to set.
/// @param value  Value to add to the property.
void Entity::add_to_base_property(std::string key, Property add_value)
{
  Property existing_value = m_properties.get(key);
  Property new_value = existing_value + add_value;
  m_properties.set(key, new_value);
}

Property Entity::get_modified_property(std::string key, Property default_value) const
{
  if (!m_properties.contains(key))
  {
    Property value = m_metadata.get_intrinsic(key, default_value);
    m_properties.set(key, Property(value));
  }

  return m_properties.get_modified(key);
}

Property Entity::get_modified_property(std::string key) const
{
  return get_modified_property(key, Property());
}

bool Entity::add_modifier(std::string key, EntityId id, ElapsedTime expires_at)
{
  return m_properties.add_modifier(key, id, expires_at);
}

size_t Entity::remove_modifier(std::string key, EntityId id)
{
  return m_properties.remove_modifier(key, id);
}

unsigned int Entity::get_quantity() const
{
  return get_base_property("quantity", Property::from(1)).as<uint32_t>();
}

void Entity::set_quantity(unsigned int quantity)
{
  set_base_property("quantity", Property::from(quantity));
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

EntityId Entity::getLocation() const
{
  return m_location;
}

bool Entity::can_see(EntityId entity)
{
  // Make sure we are able to see at all.
  if (!can_currently_see())
  {
    return false;
  }

  // Are we on a map?  Bail out if we aren't.
  MapId entity_map_id = this->get_map_id();
  MapId thing_map_id = entity->get_map_id();

  if ((entity_map_id == MapFactory::null_map_id) ||
    (thing_map_id == MapFactory::null_map_id) ||
      (entity_map_id != thing_map_id))
  {
    return false;
  }

  auto thing_location = entity->get_maptile();
  if (thing_location == nullptr)
  {
    return false;
  }

  IntVec2 thing_coords = thing_location->get_coords();

  return can_see(thing_coords);
}

bool Entity::can_see(IntVec2 coords)
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
  IntVec2 tile_coords = tile->get_coords();

  if ((tile_coords.x == coords.x) && (tile_coords.y == coords.y))
  {
    return true;
  }

  Map& game_map = GAME.get_maps().get(map_id);
  auto map_size = game_map.getSize();

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
  EntityId location = getLocation();
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

MapMemoryChunk const& Entity::get_memory_at(IntVec2 coords) const
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
            IntVec2 new_map_size = new_map.getSize();
            m_map_memory.resize(new_map_size.x * new_map_size.y);
            m_tiles_currently_seen.resize(new_map_size.x * new_map_size.y);
            /// @todo Load new map memory if it exists somewhere.
          }
        }
        this->find_seen_tiles();
        //notifyObservers(Event::Updated);
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

  EntityId location2 = location->getLocation();
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

  if (is_subtype_of("DynamicEntity") && get_modified_property("hp").as<int32_t>() <= 0)
  {
    adjectives += tr("ADJECTIVE_DEAD");
  }

  /// @todo Implement more adjectives.

  return adjectives;
}

/// @todo Figure out how to cleanly localize this.
std::string Entity::get_display_name() const
{
  return m_metadata.get_intrinsic("name").as<std::string>();
}

/// @todo Figure out how to cleanly localize this.
std::string Entity::get_display_plural() const
{
  return m_metadata.get_intrinsic("plural").as<std::string>();
}

std::string Entity::get_proper_name() const
{
  return get_modified_property("proper_name").as<std::string>();
}

void Entity::set_proper_name(std::string name)
{
  set_base_property("proper_name", Property::from(name));
}

std::string Entity::get_subject_you_or_identifying_string(ArticleChoice articles) const
{
  std::string str;

  if (is_player())
  {
    if (get_modified_property("hp").as<int32_t>() > 0)
    {
      str = tr("PRONOUN_SUBJECT_YOU");
    }
    else
    {
      str = StringTransforms::make_string_numerical_tokens_only(tr("PRONOUN_POSSESSIVE_YOU"), { tr("NOUN_CORPSE") });
    }
  }
  else
  {
    str = get_identifying_string(articles);
  }

  return str;
}

std::string Entity::get_object_you_or_identifying_string(ArticleChoice articles) const
{
  std::string str;

  if (is_player())
  {
    if (get_modified_property("hp").as<int32_t>() > 0)
    {
      str = tr("PRONOUN_OBJECT_YOU");
    }
    else
    {
      str = StringTransforms::make_string_numerical_tokens_only(tr("PRONOUN_POSSESSIVE_YOU"), { tr("NOUN_CORPSE") });
    }
  }
  else
  {
    str = get_identifying_string(articles);
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

  EntityId location = this->getLocation();
  unsigned int quantity = this->get_quantity();

  std::string name;

  bool owned;

  std::string debug_prefix;
  if (config.get("debug_show_thing_ids").as<bool>() == true)
  {
    debug_prefix = "(#" + static_cast<std::string>(get_id()) + ") ";
  }
  
  std::string adjectives;
  std::string noun;
  std::string description;
  std::string suffix;

  owned = location->is_subtype_of("DynamicEntity");
  adjectives = get_display_adjectives();

  if (quantity == 1)
  {
    noun = get_display_name();

    if (owned && (possessives == UsePossessives::Yes))
    {
      description = location->get_possessive_of(noun, adjectives);
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        description = tr("ARTICLE_DEFINITE") + " " + adjectives + " " + noun;
      }
      else
      {
        description = getIndefArt(noun) + " " + adjectives + " " + noun;
      }
    }

    if (get_proper_name().empty() == false)
    {
      suffix = tr("VERB_NAME_PP") + " " + get_proper_name();
    }
  }
  else
  {
    noun = std::to_string(get_quantity()) + " " + get_display_plural();

    if (owned && (possessives == UsePossessives::Yes))
    {
      description = location->get_possessive_of(noun, adjectives);
    }
    else
    {
      if (articles == ArticleChoice::Definite)
      {
        description = tr("ARTICLE_DEFINITE") + " " + adjectives + " " + noun;
      }
      else
      {
        description = adjectives + " " + noun;
      }
    }
  }

  name = StringTransforms::make_string_numerical_tokens_only(tr("PATTERN_DISPLAY_NAME"), {
    debug_prefix,
    description,
    suffix
  });

  return name;
}

bool Entity::is_third_person()
{
  return (GAME.get_player() == m_ref) || (get_base_property("quantity").as<uint32_t>() > 1);
}

std::string const& Entity::choose_verb(std::string const& verb12,
                                        std::string const& verb3)
{
  if ((GAME.get_player() == m_ref) || (get_base_property("quantity").as<uint32_t>() > 1))
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
  return get_modified_property("physical_mass").as<uint32_t>() * 
    get_base_property("quantity").as<uint32_t>();
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

std::string Entity::get_possessive_of(std::string owned, std::string adjectives)
{
  if (GAME.get_player() == m_ref)
  {
    return StringTransforms::make_string_numerical_tokens_only(tr("PRONOUN_POSSESSIVE_YOU"), { adjectives, owned });
  }
  else
  {
    return StringTransforms::make_string_numerical_tokens_only(tr("PATTERN_POSSESSIVE"), {
      get_identifying_string(ArticleChoice::Definite, UsePossessives::No),
      adjectives,
      owned
    });
  }
}

sf::Color Entity::get_opacity() const
{
  return get_modified_property("opacity").as<Color>();
}

bool Entity::is_opaque()
{
  auto opacity = get_opacity();
  return opacity.r >= 255 && opacity.g >= 255 && opacity.b >= 255;
}

void Entity::light_up_surroundings()
{
  EntityId location = getLocation();

  if (get_intrinsic("inventory_size").as<uint32_t>() != 0)
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

    //if (!is_opaque() || is_wielding(light) || is_wearing(light))
    if (!opaque || is_entity)
    {
      auto& inventory = get_inventory();
      for (auto iter = inventory.begin();
           iter != inventory.end();
           ++iter)
      {
        EntityId entity = iter->second;
        entity->light_up_surroundings();
      }
    }
  }

  // Use visitor pattern.
  if ((location != EntityId::Mu()) && this->get_modified_property("lit").as<bool>())
  {
    location->be_lit_by(this->get_id());
  }
}

void Entity::be_lit_by(EntityId light)
{
  auto result = call_lua_function("on_lit_by", { Property::from(light) },                                  
                                  Property::from(ActionResult::Success));
  if (result.as<ActionResult>() == ActionResult::Success)
  {
    //notifyObservers(Event::Updated);
  }

  EntityId location = getLocation();

  if (getLocation() == EntityId::Mu())
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

    //if (!is_opaque() || is_wielding(light) || is_wearing(light))
    if (!opaque || is_entity)
    {
      location->be_lit_by(light);
    }
  }
}

/// @todo Make this into an Action.
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
    EntityId entity = iter->second;
    if (m_location != EntityId::Mu())
    {
      ActionResult can_contain = m_location->can_contain(entity);

      switch (can_contain)
      {
        case ActionResult::Success:

          // Try to move this into the Entity's location.
          success = entity->move_into(m_location);
          if (success)
          {
            auto container_string = this->get_identifying_string();
            auto thing_string = entity->get_identifying_string();
            message = thing_string + this->choose_verb(" tumble", " tumbles") + " out of " + container_string + ".";
            Service<IMessageLog>::get().add(message);
          }
          else
          {
            // We couldn't move it, so just destroy it.
            auto container_string = this->get_identifying_string();
            auto thing_string = entity->get_identifying_string();
            message = thing_string + this->choose_verb(" vanish", " vanishes") + " in a puff of logic.";
            Service<IMessageLog>::get().add(message);
            entity->destroy();
          }

          //notifyObservers(Event::Updated);

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
      entity->destroy();
    }
  } // end for (contents of Entity)
}

void Entity::destroy()
{
  auto old_location = m_location;

  if (get_intrinsic("inventory_size").as<uint32_t>() != 0)
  {
    // Spill the contents of this Entity into the Entity's location.
    spill();
  }

  if (old_location != EntityId::Mu())
  {
    old_location->get_inventory().remove(m_ref);
  }

  //notifyObservers(Event::Updated);
}

/// @todo Figure out how to localize this.
std::string Entity::get_bodypart_description(BodyLocation location)
{
  uint32_t total_number = this->get_bodypart_number(location.part).as<uint32_t>();
  std::string part_name = this->get_bodypart_name(location.part).as<std::string>();
  std::string result;

  Assert("Entity", location.number < total_number, "asked for bodypart " << location.number << " of " << total_number);
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
      switch (location.number)
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
      switch (location.number)
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
      if ((location.part == BodyPart::Leg) || (location.part == BodyPart::Foot))
      {
        switch (location.number)
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
        switch (location.number)
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
      if ((location.part == BodyPart::Leg) || (location.part == BodyPart::Foot))
      {
        switch (location.number)
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
        switch (location.number)
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
    result = Ordinal::get(location.number) + " " + part_name;
  }

  return result;
}

/// @todo Have the script return an optional reason if an action can't be done.
bool Entity::can_have_action_done_by(EntityId entity, Actions::Action& action)
{
  return call_lua_function("can_have_action_" + action.get_type() + "_done_by", { Property::from(entity) },
                           Property::from(false)).as<bool>();
}

bool Entity::is_miscible_with(EntityId entity)
{
  return call_lua_function("is_miscible_with", { Property::from(entity) },
                           Property::from(false)).as<bool>();
}

BodyPart Entity::is_equippable_on() const
{
  return BodyPart::Count;
}

bool Entity::process_involuntary_actions()
{
  // Get a copy of the Entity's inventory.
  // This is because entities can be deleted/removed from the inventory
  // over the course of processing them, and this could invalidate the
  // iterator.
  Inventory temp_inventory{ m_inventory };

  // Process inventory.
  for (auto iter = temp_inventory.begin();
       iter != temp_inventory.end();
       ++iter)
  {
    EntityId entity = iter->second;
    /* bool dead = */ entity->process_involuntary_actions();
  }

  // Process self last.
  return _process_own_involuntary_actions();
}

bool Entity::process_voluntary_actions()
{
  // Get a copy of the Entity's inventory.
  // This is because entities can be deleted/removed from the inventory
  // over the course of processing them, and this could invalidate the
  // iterator.
  Inventory temp_inventory{ m_inventory };

  // Process inventory.
  for (auto iter = temp_inventory.begin();
       iter != temp_inventory.end();
       ++iter)
  {
    EntityId entity = iter->second;
    /* bool dead = */ entity->process_voluntary_actions();
  }

  // Process self last.
  return _process_own_voluntary_actions();
}

ActionResult Entity::perform_action_died()
{
  ActionResult result = call_lua_function("perform_action_died", {}, 
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

void Entity::perform_action_collided_with(EntityId actor)
{
  /* ActionResult result = */ call_lua_function("perform_action_collided_with", { Property::from(actor) },
                                                Property::from(ActionResult::Success));
  return;
}

void Entity::perform_action_collided_with_wall(Direction d, std::string tile_type)
{
  /// @todo Implement me; right now there's no way to pass one enum and one string to a Lua function.
  return;
}

ActionResult Entity::perform_intransitive_action(Actions::Action& action)
{
  ActionResult result = call_lua_function("perform_intransitive_action_" + action.get_type(), {},
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

ActionResult Entity::be_object_of(Actions::Action& action, EntityId subject)
{
  ActionResult result = call_lua_function("be_object_of_action_" + action.get_type(), { Property::from(subject) },
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

ActionResult Entity::be_object_of(Actions::Action& action, EntityId subject, EntityId target)
{
  ActionResult result = call_lua_function("be_object_of_action_" + action.get_type(), { Property::from(subject), Property::from(target) },
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

ActionResult Entity::be_object_of(Actions::Action& action, EntityId subject, Direction direction)
{
  ActionResult result = call_lua_function("be_object_of_action_" + action.get_type(), { Property::from(subject), Property::from(direction) },
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

ActionResult Entity::perform_action_hurt_by(EntityId subject)
{
  ActionResult result = call_lua_function("be_object_of_action_hurt", { Property::from(subject) },
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

ActionResult Entity::perform_action_attacked_by(EntityId subject, EntityId target)
{
  ActionResult result = call_lua_function("be_object_of_action_attack", { Property::from(subject), Property::from(target) },
                                          Property::from(ActionResult::Success)).as<ActionResult>();
  return result;
}

bool Entity::perform_action_deequipped_by(EntityId actor, BodyLocation& location)
{
  if (this->get_modified_property("bound").as<bool>())
  {
    std::string message;
    message = actor->get_identifying_string() + " cannot take off " + this->get_identifying_string() +
      "; it is magically bound to " +
      actor->get_possessive_adjective() + " " +
      actor->get_bodypart_description(location) + "!";
    Service<IMessageLog>::get().add(message);
    return false;
  }
  else
  {
    ActionResult result = call_lua_function("perform_action_deequipped_by", { Property::from(actor) },
                                            Property::from(ActionResult::Success)).as<ActionResult>();
    return was_successful(result);
  }
}

bool Entity::perform_action_equipped_by(EntityId actor, BodyLocation& location)
{
  ActionResult result = call_lua_function("perform_action_equipped_by", { Property::from(actor) },
                                          Property::from(ActionResult::Success)).as<ActionResult>();
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
  if ((get_intrinsic("inventory_size").as<uint32_t>() != 0) ||
      (other->get_intrinsic("inventory_size").as<uint32_t>() != 0))
  {
    return false;
  }

  // If the entities have the exact same properties, merge is okay.
  /// @todo Handle default properties. Right now, if a property was
  ///       queried on a Entity and pulls the default, but it was NOT queried
  ///       on the second entity, the property dictionaries will NOT match.
  ///       I have not yet found a good solution to this problem.
  auto& our_properties = this->m_properties;
  auto& other_properties = other->m_properties;

  if ((this->m_properties) == (other->m_properties))
  {
    return true;
  }

  return false;
}

ActionResult Entity::can_contain(EntityId entity)
{
  auto inventory_size_property = get_intrinsic("inventory_size");
  int32_t inventory_size = inventory_size_property.as<int32_t>();
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
    return call_lua_function("can_contain", { Property::from(entity) },
                             Property::from(ActionResult::Success)).as<ActionResult>();
  }
}

Property Entity::call_lua_function(std::string function_name,
                                   std::vector<Property> const& args,
                                   Property default_result)
{
  return the_lua_instance.call_thing_function(function_name, get_id(), args, default_result);
}

Property Entity::call_lua_function(std::string function_name,
                                   std::vector<Property> const& args)
{
  return call_lua_function(function_name, args, Property());
}

Property Entity::call_lua_function(std::string function_name,
                                   std::vector<Property> const& args,
                                   Property default_result) const
{
  return the_lua_instance.call_thing_function(function_name, get_id(), args, default_result);
}

Property Entity::call_lua_function(std::string function_name,
                                   std::vector<Property> const& args) const
{
  return call_lua_function(function_name, args, Property());
}

void Entity::setLocation(EntityId target)
{
  m_location = target;
}

std::unordered_set<EventID> Entity::registeredEvents() const
{
  auto events = Subject::registeredEvents();
  /// @todo Add our own events here
  return events;
}

// *** PROTECTED METHODS ******************************************************

bool Entity::_process_own_involuntary_actions()
{
  int counter_busy = get_base_property("counter_busy").as<int32_t>();

  // Is this an entity that is now dead?
  if (is_subtype_of("DynamicEntity") && (get_modified_property("hp").as<int32_t>() <= 0))
  {
    // Did the entity JUST die?
    if (get_modified_property("dead").as<bool>() != true)
    {
      // Perform the "die" action.
      // (This sets the "dead" property and clears out any pending actions.)
      std::unique_ptr<Actions::Action> dieAction(NEW Actions::ActionDie(get_id()));
      this->queue_involuntary_action(std::move(dieAction));
    }
  }

  // If actions are pending...
  if (!m_pending_voluntary_actions.empty())
  {
    // Process the front action.
    std::unique_ptr<Actions::Action>& action = m_pending_involuntary_actions.front();
    bool action_done = action->process({});
    if (action_done)
    {
      CLOG(TRACE, "Entity") << "Entity " <<
        get_id() << "( " <<
        get_type() << "): Involuntary Action " <<
        action->get_type() << " is done, popping";

      m_pending_involuntary_actions.pop_front();
    }

    /// @todo This needs to be changed so it only is called if the Action
    ///       materially affected the entity in some way. Two ways to do this
    ///       that I can see:
    ///         1) The Action calls notifyObservers. Requires the Action
    ///            to have access to that method; right now it doesn't.
    ///         2) The Action returns some sort of indication that the Entity
    ///            was modified as a result. This could be done by changing
    ///            the return type from a bool to a struct of some sort.
    //notifyObservers(Event::Updated);

  } // end if (actions pending)

  return true;
}

bool Entity::_process_own_voluntary_actions()
{
  int counter_busy = get_base_property("counter_busy").as<int32_t>();

  // Is this an entity that is now dead?
  if (is_subtype_of("DynamicEntity") && (get_modified_property("hp").as<int32_t>() <= 0))
  {
    // Did the entity JUST die?
    if (get_modified_property("dead").as<bool>() != true)
    {
      // Perform the "die" action.
      // (This sets the "dead" property and clears out any pending actions.)
      std::unique_ptr<Actions::Action> dieAction(NEW Actions::ActionDie(get_id()));
      this->queue_involuntary_action(std::move(dieAction));
    }
  }

  // If this entity is busy...
  if (counter_busy > 0)
  {
    // Decrement busy counter.
    add_to_base_property("counter_busy", Property::from(-1));
  }
  // Otherwise if actions are pending...
  else if (!m_pending_voluntary_actions.empty())
  {
    // Process the front action.
    std::unique_ptr<Actions::Action>& action = m_pending_voluntary_actions.front();
    bool action_done = action->process({});
    if (action_done)
    {
      CLOG(TRACE, "Entity") << "Entity " <<
        get_id() << "( " <<
        get_type() << "): Action " <<
        action->get_type() << " is done, popping";

      m_pending_voluntary_actions.pop_front();
    }

    /// @todo This needs to be changed so it only is called if the Action
    ///       materially affected the entity in some way. Two ways to do this
    ///       that I can see:
    ///         1) The Action calls notifyObservers. Requires the Action
    ///            to have access to that method; right now it doesn't.
    ///         2) The Action returns some sort of indication that the Entity
    ///            was modified as a result. This could be done by changing
    ///            the return type from a bool to a struct of some sort.
    //notifyObservers(Event::Updated);

  } // end if (actions pending)
  // Otherwise if there are no other pending actions...
  else
  {
    // If entity is not the player, call the Lua process function on this Entity.
    if (!is_player())
    {
      ActionResult result = call_lua_function("process", {}).as<ActionResult>();
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
  Assert("Entity", octant >= 1 && octant <= 8, "Octant" << octant << "passed in is not between 1 and 8 inclusively");
  IntVec2 new_coords;
  //int x = 0;
  //int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (is_inside_another_thing())
  {
    return;
  }

  MapTile* tile = get_maptile();
  IntVec2 tile_coords = tile->get_coords();
  Map& game_map = GAME.get_maps().get(get_map_id());

  static const int mv = 128;
  static constexpr int mw = (mv * mv);

  std::function< bool(RealVec2, RealVec2, float) > loop_condition;
  Direction dir;
  std::function< float(RealVec2, RealVec2) > recurse_slope;
  std::function< float(RealVec2, RealVec2) > loop_slope;

  switch (octant)
  {
    case 1:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
      break;

    case 2:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Northeast.half(), b); };
      break;

    case 3:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
      break;

    case 4:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      break;

    case 5:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Southeast.half(), b); };
      break;

    case 6:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_slope(a + Direction::Southwest.half(), b); };
      break;

    case 7:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
      break;

    case 8:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](RealVec2 a, RealVec2 b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](RealVec2 a, RealVec2 b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      break;

    default:
      break;
  }

  while (loop_condition(to_v2f(new_coords), to_v2f(tile_coords), slope_B))
  {
    if (calc_vis_distance(new_coords, tile_coords) <= mw)
    {
      if (game_map.tile_is_opaque(new_coords))
      {
        if (!game_map.tile_is_opaque(new_coords + (IntVec2)dir))
        {
          do_recursive_visibility(octant, depth + 1, slope_A, recurse_slope(to_v2f(new_coords), to_v2f(tile_coords)));
        }
      }
      else
      {
        if (game_map.tile_is_opaque(new_coords + (IntVec2)dir))
        {
          slope_A = loop_slope(to_v2f(new_coords), to_v2f(tile_coords));
        }
      }
      m_tiles_currently_seen[game_map.get_index(new_coords)] = true;

      MapMemoryChunk new_memory{ game_map.get_tile(new_coords).get_tile_type(),
                                 GAME.get_game_clock() };
      m_map_memory[game_map.get_index(new_coords)] = new_memory;
    }
    new_coords -= (IntVec2)dir;
  }
  new_coords += (IntVec2)dir;

  if ((depth < mv) && (!game_map.get_tile(new_coords).is_opaque()))
  {
    do_recursive_visibility(octant, depth + 1, slope_A, slope_B);
  }
}

MapTile* Entity::_get_maptile() const
{
  return m_map_tile;
}