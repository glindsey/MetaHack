#include "stdafx.h"

#include "Thing.h"

#include "App.h"
#include "ConfigSettings.h"
#include "Direction.h"
#include "GameState.h"
#include "Gender.h"
#include "IntegerRange.h"
#include "Inventory.h"
#include "Map.h"
#include "MapTile.h"
#include "MapTileMetadata.h"
#include "MathUtils.h"
#include "MessageLog.h"
#include "Metadata.h"
#include "Ordinal.h"
#include "ThingManager.h"
#include "TileSheet.h"

// Static member initialization.
sf::Color const Thing::wall_outline_color_ = sf::Color(255, 255, 255, 64);

Thing::Thing(GameState& game, Metadata& metadata, ThingId ref)
  : 
  m_game{ game },
  m_metadata{ metadata },
  m_properties{},
  m_ref{ ref },
  m_location{ ThingId::Mu() },
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

Thing::Thing(GameState& game, MapTile* map_tile, Metadata& metadata, ThingId ref)
  : 
  m_game{ game },
  m_metadata{ metadata },
  m_properties{},
  m_ref{ ref },
  m_location{ ThingId::Mu() },
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

Thing::Thing(Thing const& original, ThingId ref)
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

void Thing::initialize()
{
  SET_UP_LOGGER("Thing", true);

  /// Get our maximum HP. (The Lua script will automatically pick it from a range.)
  int max_hp = m_metadata.get_intrinsic<int>("maxhp");
  set_base_property<int>("maxhp", max_hp);

  /// Also set our HP to that value.
  set_base_property<int>("hp", max_hp);
}

Thing::~Thing()
{
}

void Thing::queue_action(std::unique_ptr<Action> pAction)
{
  m_pending_actions.push_back(std::move(pAction));
}

bool Thing::action_is_pending() const
{
  return !(m_pending_actions.empty());
}

bool Thing::action_is_in_progress()
{
  return (get_base_property<int>("counter_busy") > 0);
}

ThingId Thing::get_wielding_in(unsigned int& hand)
{
  if (m_wielded_items.count(hand) == 0)
  {
    return ThingId::Mu();
  }
  else
  {
    return m_wielded_items[hand];
  }
}

bool Thing::is_wielding(ThingId thing)
{
  unsigned int dummy;
  return is_wielding(thing, dummy);
}

bool Thing::is_wielding(ThingId thing, unsigned int& hand)
{
  if (thing == ThingId::Mu())
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

bool Thing::has_equipped(ThingId thing)
{
  WearLocation dummy;
  return has_equipped(thing, dummy);
}

bool Thing::has_equipped(ThingId thing, WearLocation& location)
{
  if (thing == ThingId::Mu())
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

bool Thing::can_reach(ThingId thing)
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

bool Thing::is_adjacent_to(ThingId thing)
{
  // Get the coordinates we are at.
  MapTile* our_maptile = get_maptile();
  MapTile* thing_maptile = thing->get_maptile();
  if ((our_maptile == nullptr) || (thing_maptile == nullptr))
  {
    return false;
  }

  sf::Vector2i const& our_coords = our_maptile->get_coords();
  sf::Vector2i const& thing_coords = thing_maptile->get_coords();

  return adjacent(our_coords, thing_coords);
}

bool Thing::do_die()
{
  /// @todo Handle stuff like auto-activating life-saving items here.
  /// @todo Pass in the cause of death somehow.

  ActionResult result = perform_action_died();
  StringDisplay message;

  switch (result)
  {
    case ActionResult::Success:
      if (this->is_player())
      {
        message = this->get_you_or_identifying_string() + L" die...";
        the_message_log.add(message);
      }
      else
      {
        bool living = get_modified_property<bool>("living");
        if (living)
        {
          message = this->get_you_or_identifying_string() + L" " +
            this->choose_verb(L"are", L"is") + L" killed!";
        }
        else
        {
          message = this->get_you_or_identifying_string() + L" " +
            this->choose_verb(L"are", L"is") + L" destroyed!";
        }
        the_message_log.add(message);
      }

      // Set the property saying the entity is dead.
      set_base_property<bool>("dead", true);

      // Clear any pending actions.
      m_pending_actions.clear();

      return true;
    case ActionResult::Failure:
    default:
      message = this->get_you_or_identifying_string() +
        this->choose_verb(L" manage", L" manages") + L" to avoid dying.";
      the_message_log.add(message);
      return false;
  }
}

bool Thing::do_attack(ThingId thing, unsigned int& action_time)
{
  StringDisplay message;

  bool reachable = this->is_adjacent_to(thing);
  /// @todo deal with Entities in your Inventory -- WTF do you do THEN?

  if (reachable)
  {
    /// @todo Write attack code.
    message = this->get_you_or_identifying_string() + L" " +
      this->choose_verb(L"try", L"tries") + L" to attack " +
      thing->get_identifying_string() + L", but " +
      this->choose_verb(L"are", L"is") + L" stopped by the programmer's procrastination!";
    the_message_log.add(message);
  }

  return false;
}

ActionResult Thing::can_deequip(ThingId thing, unsigned int& action_time)
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

  /// @todo Finish Thing::can_deequip code
  return ActionResult::Success;
}

bool Thing::do_deequip(ThingId thing, unsigned int& action_time)
{
  StringDisplay message;
  ActionResult deequip_try = this->can_deequip(thing, action_time);
  StringDisplay thing_name = thing->get_identifying_string();

  message = this->get_you_or_identifying_string() + L" " +
    this->choose_verb(L"try", L"tries") +
    L" to take off " + thing_name;
  the_message_log.add(message);

  switch (deequip_try)
  {
    case ActionResult::Success:
    {
      // Get the body part this item is equipped on.
      WearLocation location;
      this->has_equipped(thing, location);

      if (thing->perform_action_deequipped_by(m_ref, location))
      {
        set_worn(ThingId::Mu(), location);

        StringDisplay wear_desc = get_bodypart_description(location.part, location.number);
        message = this->get_you_or_identifying_string() + L" " +
          this->choose_verb(L"are", L"is") + L" no longer wearing " + thing_name +
          L" on " + this->get_possessive() + L" " + wear_desc + L".";
        the_message_log.add(message);
        return true;
      }
    }
    break;

    case ActionResult::FailureItemNotEquipped:
    {
      message = this->get_you_or_identifying_string() + L" " +
        this->choose_verb(L"are", L"is") + L" not wearing " + thing_name + L".";
      the_message_log.add(message);
      return true;
    }
    break;

    default:
      CLOG(WARNING, "Thing") << "Unknown ActionResult " << deequip_try;
      break;
  }

  return false;
}

ActionResult Thing::can_equip(ThingId thing, unsigned int& action_time)
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
    return ActionResult::FailureThingOutOfReach;
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

  /// @todo Finish Thing::can_equip code
  return ActionResult::Success;
}

bool Thing::do_equip(ThingId thing, unsigned int& action_time)
{
  StringDisplay message;

  ActionResult equip_try = this->can_equip(thing, action_time);
  StringDisplay thing_name = thing->get_identifying_string();

  switch (equip_try)
  {
    case ActionResult::Success:
    {
      WearLocation location;

      if (thing->perform_action_equipped_by(m_ref, location))
      {
        set_worn(thing, location);

        StringDisplay wear_desc = get_bodypart_description(location.part,
                                                           location.number);
        message = this->get_you_or_identifying_string() + L" " +
          this->choose_verb(L" are", L" is") +
          L" now wearing " + thing_name +
          L" on " + this->get_possessive() + L" " + wear_desc + L".";
        the_message_log.add(message);
        return true;
      }
    }
    break;

    case ActionResult::FailureSelfReference:
      if (is_player())
      {
        message = L"To equip yourself, choose what you want to equip first.";
      }
      else
      {
        message = this->get_you_or_identifying_string() + L" " +
          this->choose_verb(L"try", L"tries") +
          L" to equip " + this->get_reflexive_pronoun() +
          L", which seriously shouldn't happen.";
        CLOG(WARNING, "Thing") << "NPC tried to equip self!?";
      }
      the_message_log.add(message);
      break;

    case ActionResult::FailureThingOutOfReach:
    {
      message = this->get_you_or_identifying_string() + L" " +
        this->choose_verb(L"try", L"tries") +
        L" to equip " + thing_name + L".";
      the_message_log.add(message);

      message = thing_name + L" is not in " + this->get_possessive() + L" inventory.";
      the_message_log.add(message);
    }
    break;

    case ActionResult::FailureItemNotEquippable:
    {
      message = this->get_you_or_identifying_string() + L" " +
        this->choose_verb(L"try", L"tries") +
        L" to equip " + thing_name + L".";
      the_message_log.add(message);

      message = thing_name + L" is not an equippable item.";
      the_message_log.add(message);
    }
    break;

    default:
      CLOG(WARNING, "Thing") << "Unknown ActionResult " << equip_try;
      break;
  }

  return false;
}

void Thing::set_wielded(ThingId thing, unsigned int hand)
{
  if (thing == ThingId::Mu())
  {
    m_wielded_items.erase(hand);
  }
  else
  {
    m_wielded_items[hand] = thing;
  }
}

void Thing::set_worn(ThingId thing, WearLocation location)
{
  if (thing == ThingId::Mu())
  {
    m_equipped_items.erase(location);
  }
  else
  {
    m_equipped_items[location] = thing;
  }
}

bool Thing::can_currently_see()
{
  return get_modified_property<bool>("can_see", false);
}

bool Thing::can_currently_move()
{
  return get_modified_property<bool>("can_move", false);
}

void Thing::set_gender(Gender gender)
{
  m_gender = gender;
}

Gender Thing::get_gender() const
{
  return m_gender;
}

Gender Thing::get_gender_or_you() const
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

/// Get the number of a particular body part the Entity has.
unsigned int Thing::get_bodypart_number(BodyPart part) const
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

/// Get the appropriate body part name for the Entity.
StringDisplay Thing::get_bodypart_name(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic<StringDisplay>("bodypart_body_name");
    case BodyPart::Skin:
      return get_intrinsic<StringDisplay>("bodypart_skin_name");
    case BodyPart::Head:
      return get_intrinsic<StringDisplay>("bodypart_head_name");
    case BodyPart::Ear:
      return get_intrinsic<StringDisplay>("bodypart_ear_name");
    case BodyPart::Eye:
      return get_intrinsic<StringDisplay>("bodypart_eye_name");
    case BodyPart::Nose:
      return get_intrinsic<StringDisplay>("bodypart_nose_name");
    case BodyPart::Mouth:
      return get_intrinsic<StringDisplay>("bodypart_mouth_name");
    case BodyPart::Neck:
      return get_intrinsic<StringDisplay>("bodypart_neck_name");
    case BodyPart::Chest:
      return get_intrinsic<StringDisplay>("bodypart_chest_name");
    case BodyPart::Arm:
      return get_intrinsic<StringDisplay>("bodypart_arm_name");
    case BodyPart::Hand:
      return get_intrinsic<StringDisplay>("bodypart_hand_name");
    case BodyPart::Leg:
      return get_intrinsic<StringDisplay>("bodypart_leg_name");
    case BodyPart::Foot:
      return get_intrinsic<StringDisplay>("bodypart_foot_name");
    case BodyPart::Wing:
      return get_intrinsic<StringDisplay>("bodypart_wing_name");
    case BodyPart::Tail:
      return get_intrinsic<StringDisplay>("bodypart_tail_name");
    default:
      return L"squeedlyspooch (unknown BodyPart)";
  }
}

/// Get the appropriate body part plural for the Entity.
StringDisplay Thing::get_bodypart_plural(BodyPart part) const
{
  switch (part)
  {
    case BodyPart::Body:
      return get_intrinsic<StringDisplay>("bodypart_body_plural", get_bodypart_name(BodyPart::Body) + L"s");
    case BodyPart::Skin:
      return get_intrinsic<StringDisplay>("bodypart_skin_plural", get_bodypart_name(BodyPart::Skin) + L"s");
    case BodyPart::Head:
      return get_intrinsic<StringDisplay>("bodypart_head_plural", get_bodypart_name(BodyPart::Head) + L"s");
    case BodyPart::Ear:
      return get_intrinsic<StringDisplay>("bodypart_ear_plural", get_bodypart_name(BodyPart::Ear) + L"s");
    case BodyPart::Eye:
      return get_intrinsic<StringDisplay>("bodypart_eye_plural", get_bodypart_name(BodyPart::Eye) + L"s");
    case BodyPart::Nose:
      return get_intrinsic<StringDisplay>("bodypart_nose_plural", get_bodypart_name(BodyPart::Nose) + L"s");
    case BodyPart::Mouth:
      return get_intrinsic<StringDisplay>("bodypart_mouth_plural", get_bodypart_name(BodyPart::Mouth) + L"s");
    case BodyPart::Neck:
      return get_intrinsic<StringDisplay>("bodypart_neck_plural", get_bodypart_name(BodyPart::Neck) + L"s");
    case BodyPart::Chest:
      return get_intrinsic<StringDisplay>("bodypart_chest_plural", get_bodypart_name(BodyPart::Chest) + L"s");
    case BodyPart::Arm:
      return get_intrinsic<StringDisplay>("bodypart_arm_plural", get_bodypart_name(BodyPart::Arm) + L"s");
    case BodyPart::Hand:
      return get_intrinsic<StringDisplay>("bodypart_hand_plural", get_bodypart_name(BodyPart::Hand) + L"s");
    case BodyPart::Leg:
      return get_intrinsic<StringDisplay>("bodypart_leg_plural", get_bodypart_name(BodyPart::Leg) + L"s");
    case BodyPart::Foot:
      return get_intrinsic<StringDisplay>("bodypart_foot_plural", get_bodypart_name(BodyPart::Foot) + L"s");
    case BodyPart::Wing:
      return get_intrinsic<StringDisplay>("bodypart_wing_plural", get_bodypart_name(BodyPart::Wing) + L"s");
    case BodyPart::Tail:
      return get_intrinsic<StringDisplay>("bodypart_tail_plural", get_bodypart_name(BodyPart::Tail) + L"s");
    default:
      return L"squeedlyspooches (unknown BodyParts)";
  }
}

bool Thing::is_player() const
{
  return (GAME.get_player() == m_ref);
}

StringKey const& Thing::get_type() const
{
  return m_metadata.get_type();
}

StringKey const& Thing::get_parent_type() const
{
  return m_metadata.get_intrinsic<StringKey>("parent");
}

bool Thing::is_subtype_of(StringKey that_type) const
{
  StringKey this_type = get_type();
  return GAME.get_things().first_is_subtype_of_second(this_type, that_type);
}

bool Thing::add_modifier(StringKey key, ThingId id, unsigned int expiration_ticks)
{
  return m_properties.add_modifier(key, id, expiration_ticks);
}

unsigned int Thing::remove_modifier(StringKey key, ThingId id)
{
  return m_properties.remove_modifier(key, id);
}

unsigned int Thing::get_quantity()
{
  return get_base_property<unsigned int>("quantity", 1);
}

void Thing::set_quantity(unsigned int quantity)
{
  set_base_property<unsigned int>("quantity", quantity);
}

ThingId Thing::get_id() const
{
  return m_ref;
}

ThingId Thing::get_root_location() const
{
  if (m_location == ThingId::Mu())
  {
    return m_ref;
  }
  else
  {
    auto location = m_location;
    return location->get_root_location();
  }
}

ThingId Thing::get_location() const
{
  return m_location;
}

bool Thing::can_see(ThingId thing)
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

  sf::Vector2i thing_coords = thing_location->get_coords();

  return can_see(thing_coords.x, thing_coords.y);
}

bool Thing::can_see(sf::Vector2i coords)
{
  return this->can_see(coords.x, coords.y);
}

bool Thing::can_see(int xTile, int yTile)
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

  // If the coordinates are where we are, then yes, we can indeed see the tile.
  auto tile = get_maptile();
  if (tile == nullptr)
  {
    return false;
  }

  sf::Vector2i tile_coords = tile->get_coords();

  if ((tile_coords.x == xTile) && (tile_coords.y == yTile))
  {
    return true;
  }

  Map& game_map = GAME.get_maps().get(map_id);

  if (can_currently_see())
  {
    // Return seen data.
    return m_tiles_currently_seen[game_map.get_index(xTile, yTile)];
  }
  else
  {
    return false;
  }
}

void Thing::find_seen_tiles()
{
  //sf::Clock elapsed;

  //elapsed.restart();

  // Are we on a map?  Bail out if we aren't.
  ThingId location = get_location();
  if (location == ThingId::Mu())
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

  for (int n = 1; n <= 8; ++n)
  {
    do_recursive_visibility(n);
  }
}

StringKey Thing::get_memory_at(int x, int y) const
{
  if (this->get_map_id() == MapFactory::null_map_id)
  {
    return "???";
  }

  Map& game_map = GAME.get_maps().get(this->get_map_id());
  return m_map_memory[game_map.get_index(x, y)];
}

StringKey Thing::get_memory_at(sf::Vector2i coords) const
{
  return this->get_memory_at(coords.x, coords.y);
}

void Thing::add_memory_vertices_to(sf::VertexArray& vertices,
                                   int x, int y)
{
  MapId map_id = this->get_map_id();
  if (map_id == MapFactory::null_map_id)
  {
    return;
  }
  Map& game_map = GAME.get_maps().get(map_id);

  static sf::Vertex new_vertex;
  float ts = the_config.get<float>("map_tile_size");
  float ts2 = ts * 0.5f;

  sf::Vector2f location(x * ts, y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);

  StringKey tile_type = m_map_memory[game_map.get_index(x, y)];
  if (tile_type == "") { tile_type = "MTUnknown"; }
  Metadata* tile_metadata = &(m_game.get_metadata_collection("maptile").get(tile_type));

  /// @todo Call a script to handle selecting a tile other than the one
  ///       in the upper-left corner.
  sf::Vector2u tile_coords = tile_metadata->get_tile_coords();

  TileSheet::add_quad(vertices,
                      tile_coords, sf::Color::White,
                      vNW, vNE,
                      vSW, vSE);
}

bool Thing::move_into(ThingId new_location)
{
  MapId old_map_id = this->get_map_id();
  ThingId old_location = m_location;

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
        if (m_location != ThingId::Mu())
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
            sf::Vector2i new_map_size = new_map.get_size();
            m_map_memory.resize(new_map_size.x * new_map_size.y);
            m_tiles_currently_seen.resize(new_map_size.x * new_map_size.y);
            /// @todo Load new map memory if it exists somewhere.
          }
        }
        this->find_seen_tiles();
        return true;
      } // end if (add to new inventory was successful)

      break;
    default:
      break;
  } // end switch (ActionResult)

  return false;
}

Inventory& Thing::get_inventory()
{
  return m_inventory;
}

bool Thing::is_inside_another_thing() const
{
  ThingId location = m_location;
  if (location == ThingId::Mu())
  {
    // Thing is a part of the MapTile such as the floor.
    return false;
  }

  ThingId location2 = location->get_location();
  if (location2 == ThingId::Mu())
  {
    // Thing is directly on the floor.
    return false;
  }
  return true;
}

MapTile* Thing::get_maptile() const
{
  ThingId location = m_location;

  if (location == ThingId::Mu())
  {
    return _get_maptile();
  }
  else
  {
    return location->get_maptile();
  }
}

MapId Thing::get_map_id() const
{
  ThingId location = m_location;

  if (location == ThingId::Mu())
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

StringDisplay Thing::get_display_name() const
{
  /// @todo Implement adding adjectives.
  return m_metadata.get_intrinsic<StringDisplay>("name");
}

StringDisplay Thing::get_display_plural() const
{
  return m_metadata.get_intrinsic<StringDisplay>("plural");
}

StringDisplay Thing::get_proper_name()
{
  return get_modified_property<StringDisplay>("proper_name");
}

void Thing::set_proper_name(StringDisplay name)
{
  set_base_property<StringDisplay>("proper_name", name);
}

StringDisplay Thing::get_identifying_string_without_possessives(bool definite)
{
  ThingId location = this->get_location();
  unsigned int quantity = this->get_quantity();

  StringDisplay name;

  StringDisplay article;
  StringDisplay adjectives;
  StringDisplay noun;
  StringDisplay suffix;

  if (is_player())
  {
    if (get_modified_property<int>("hp") > 0)
    {
      return L"you";
    }
    else
    {
      return L"your corpse";
    }
  }

  if (quantity == 1)
  {
    noun = get_display_name();

    if (definite)
    {
      article = L"the ";
    }
    else
    {
      article = getIndefArt(noun) + L" ";
    }

    if (get_proper_name().empty() == false)
    {
      suffix = L" named " + get_proper_name();
    }
  }
  else
  {
    noun = get_display_plural();

    if (definite)
    {
      article = L"the ";
    }
    article += std::to_wstring(get_quantity()) + L" ";
  }

  if (is_subtype_of("Entity") && get_modified_property<int>("hp") <= 0)
  {
    adjectives += L"dead ";
  }

  name = article + adjectives + noun + suffix;

  return name;
}

StringDisplay Thing::get_you_or_identifying_string(bool definite)
{
  if (is_player())
  {
    if (get_modified_property<int>("hp") > 0)
    {
      return L"you";
    }
    else
    {
      return L"your corpse";
    }
  }

  return get_identifying_string(definite);
}

StringDisplay Thing::get_self_or_identifying_string(ThingId other, bool definite)
{
  if (other == get_id())
  {
    return get_reflexive_pronoun();
  }

  return get_identifying_string(definite);
}

StringDisplay Thing::get_identifying_string(bool definite)
{
  ThingId location = this->get_location();
  unsigned int quantity = this->get_quantity();

  StringDisplay name;

  bool owned;

  StringDisplay article;
  StringDisplay adjectives;
  StringDisplay noun;
  StringDisplay suffix;

  owned = location->is_subtype_of("Entity");

  if (quantity == 1)
  {
    noun = get_display_name();

    if (owned)
    {
      article = location->get_possessive() + L" ";
    }
    else
    {
      if (definite)
      {
        article = L"the ";
      }
      else
      {
        article = getIndefArt(noun) + L" ";
      }
    }

    if (get_proper_name().empty() == false)
    {
      suffix = L" named " + get_proper_name();
    }
  }
  else
  {
    noun = get_display_plural();

    if (owned)
    {
      article = location->get_possessive() + L" ";
    }
    else
    {
      if (definite)
      {
        article = L"the ";
      }

      article += std::to_wstring(get_quantity()) + L" ";
    }
  }

  if (is_subtype_of("Entity") && get_modified_property<int>("hp") <= 0)
  {
    adjectives += L"dead ";
  }

  name = article + adjectives + noun + suffix;

  return name;
}

StringDisplay const& Thing::choose_verb(StringDisplay const& verb12,
                                        StringDisplay const& verb3)
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

int Thing::get_mass()
{
  return get_modified_property<int>("physical_mass") * get_base_property<unsigned int>("quantity");
}

StringDisplay const& Thing::get_subject_pronoun() const
{
  return getSubjPro(get_gender_or_you());
}

StringDisplay const& Thing::get_object_pronoun() const
{
  return getObjPro(get_gender_or_you());
}

StringDisplay const& Thing::get_reflexive_pronoun() const
{
  return getRefPro(get_gender_or_you());
}

StringDisplay const& Thing::get_possessive_adjective() const
{
  return getPossAdj(get_gender_or_you());
}

StringDisplay const& Thing::get_possessive_pronoun() const
{
  return getPossPro(get_gender_or_you());
}

StringDisplay Thing::get_possessive()
{
  if (GAME.get_player() == m_ref)
  {
    return L"your";
  }
  else
  {
    return get_identifying_string_without_possessives(true) + L"'s";
  }
}

sf::Vector2u Thing::get_tile_sheet_coords(int frame)
{
  /// Get tile coordinates on the sheet.
  sf::Vector2u start_coords = m_metadata.get_tile_coords();

  /// Call the Lua function to get the offset (tile to choose).
  sf::Vector2u offset = call_lua_function<sf::Vector2u>("get_tile_offset", { frame });

  /// Add them to get the resulting coordinates.
  sf::Vector2u tile_coords = start_coords + offset;

  return tile_coords;
}

void Thing::add_floor_vertices_to(sf::VertexArray& vertices,
                                  bool use_lighting,
                                  int frame)
{
  sf::Vertex new_vertex;
  float ts = the_config.get<float>("map_tile_size");
  float ts2 = ts * 0.5f;

  MapTile* root_tile = this->get_maptile();
  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  sf::Vector2i const& coords = root_tile->get_coords();

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile->get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  sf::Vector2f location(coords.x * ts, coords.y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);
  sf::Vector2u tile_coords = this->get_tile_sheet_coords(frame);

  TileSheet::add_quad(vertices,
                      tile_coords, thing_color,
                      vNW, vNE,
                      vSW, vSE);
}

void Thing::draw_to(sf::RenderTarget& target,
                    sf::Vector2f target_coords,
                    unsigned int target_size,
                    bool use_lighting,
                    int frame)
{
  MapTile* root_tile = this->get_maptile();

  if (!root_tile)
  {
    // Item's root location isn't a MapTile, so it can't be rendered.
    return;
  }

  sf::RectangleShape rectangle;
  sf::IntRect texture_coords;

  if (target_size == 0)
  {
    target_size = the_config.get<unsigned int>("map_tile_size");
  }

  unsigned int tile_size = the_config.get<unsigned int>("map_tile_size");

  sf::Vector2u tile_coords = this->get_tile_sheet_coords(frame);
  texture_coords.left = tile_coords.x * tile_size;
  texture_coords.top = tile_coords.y * tile_size;
  texture_coords.width = tile_size;
  texture_coords.height = tile_size;

  sf::Color thing_color;
  if (use_lighting)
  {
    thing_color = root_tile->get_light_level();
  }
  else
  {
    thing_color = sf::Color::White;
  }

  rectangle.setPosition(target_coords);
  rectangle.setSize(sf::Vector2f(static_cast<float>(target_size),
                                 static_cast<float>(target_size)));
  rectangle.setTexture(&(the_tilesheet.getTexture()));
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

  target.draw(rectangle);
}

bool Thing::is_opaque()
{
  return
    (get_modified_property<int>("opacity_red") >= 255) &&
    (get_modified_property<int>("opacity_green") >= 255) &&
    (get_modified_property<int>("opacity_blue") >= 255);
}

void Thing::light_up_surroundings()
{
  if (get_intrinsic<int>("inventory_size") != 0)
  {
    /// @todo Figure out how we want to handle light sources.
    ///       If we want to be more accurate, the light should only
    ///       be able to be seen when a character is wielding or has
    ///       equipped it. If we want to be easier, the light should
    ///       shine simply if it's in the player's inventory.

    //if (!is_opaque() || is_wielding(light) || has_equipped(light))
    if (!is_opaque() || get_location()->is_subtype_of("Entity"))
    {
      auto& things = get_inventory().get_things();
      for (auto& thing_pair : things)
      {
        ThingId thing = thing_pair.second;
        thing->light_up_surroundings();
      }
    }
  }

  ThingId location = get_location();

  // Use visitor pattern.
  if ((location != ThingId::Mu()) && this->get_modified_property<bool>("lit"))
  {
    location->be_lit_by(this->get_id());
  }
}

void Thing::be_lit_by(ThingId light)
{
  call_lua_function<ActionResult, ThingId>("on_lit_by", { light }, ActionResult::Success);

  if (get_location() == ThingId::Mu())
  {
    GAME.get_maps().get(get_map_id()).add_light(light);
  }

  /// @todo Figure out how we want to handle light sources.
  ///       If we want to be more accurate, the light should only
  ///       be able to be seen when a character is wielding or has
  ///       equipped it. If we want to be easier, the light should
  ///       shine simply if it's in the player's inventory.

  //if (!is_opaque() || is_wielding(light) || has_equipped(light))
  ThingId location = get_location();
  if (!is_opaque() || location->is_subtype_of("Entity"))
  {
    if (location != ThingId::Mu())
    {
      location->be_lit_by(light);
    }
  }
}

void Thing::spill()
{
  Inventory& inventory = get_inventory();
  ThingMap const& things = inventory.get_things();
  StringDisplay message;
  bool success = false;

  // Step through all contents of this Thing.
  for (ThingPair thing_pair : things)
  {
    ThingId thing = thing_pair.second;
    if (m_location != ThingId::Mu())
    {
      ActionResult can_contain = m_location->can_contain(thing);

      switch (can_contain)
      {
        case ActionResult::Success:

          // Try to move this into the Thing's location.
          success = thing->move_into(m_location);
          if (success)
          {
            auto container_string = this->get_identifying_string();
            auto thing_string = thing->get_identifying_string();
            message = thing_string + this->choose_verb(L" tumble", L" tumbles") + L" out of " + container_string + L".";
            the_message_log.add(message);
          }
          else
          {
            // We couldn't move it, so just destroy it.
            auto container_string = this->get_identifying_string();
            auto thing_string = thing->get_identifying_string();
            message = thing_string + this->choose_verb(L" vanish", L" vanishes") + L" in a puff of logic.";
            the_message_log.add(message);
            thing->destroy();
          }

          break;

        case ActionResult::FailureInventoryFull:
          /// @todo Handle the situation where the Thing's container can't hold the Thing.
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
  } // end for (contents of Thing)
}

void Thing::destroy()
{
  auto old_location = m_location;

  if (get_intrinsic<int>("inventory_size") != 0)
  {
    // Spill the contents of this Thing into the Thing's location.
    spill();
  }

  if (old_location != ThingId::Mu())
  {
    old_location->get_inventory().remove(m_ref);
  }
}

StringDisplay Thing::get_bodypart_description(BodyPart part,
                                              unsigned int number)
{
  unsigned int total_number = this->get_bodypart_number(part);
  StringDisplay part_name = this->get_bodypart_name(part);
  StringDisplay result;

  ASSERT_CONDITION(number < total_number);
  switch (total_number)
  {
    case 0: // none of them!?  shouldn't occur!
      result = L"non-existent " + part_name;
      CLOG(WARNING, "Thing") << "Request for description of " << result << "!?";
      break;

    case 1: // only one of them
      result = part_name;
      break;

    case 2: // assume a right and left one.
      switch (number)
      {
        case 0:
          result = L"right " + part_name;
          break;
        case 1:
          result = L"left " + part_name;
          break;
        default:
          break;
      }
      break;

    case 3: // assume right, center, and left.
      switch (number)
      {
        case 0:
          result = L"right " + part_name;
          break;
        case 1:
          result = L"center " + part_name;
          break;
        case 2:
          result = L"left " + part_name;
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
            result = L"front right " + part_name;
            break;
          case 1:
            result = L"front left " + part_name;
            break;
          case 2:
            result = L"rear right " + part_name;
            break;
          case 3:
            result = L"rear left " + part_name;
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
            result = L"upper right " + part_name;
            break;
          case 1:
            result = L"upper left " + part_name;
            break;
          case 2:
            result = L"lower right " + part_name;
            break;
          case 3:
            result = L"lower left " + part_name;
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
            result = L"front right " + part_name;
            break;
          case 1:
            result = L"front left " + part_name;
            break;
          case 2:
            result = L"middle right " + part_name;
            break;
          case 3:
            result = L"middle left " + part_name;
            break;
          case 4:
            result = L"rear right " + part_name;
            break;
          case 5:
            result = L"rear left " + part_name;
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
            result = L"upper right " + part_name;
            break;
          case 1:
            result = L"upper left " + part_name;
            break;
          case 2:
            result = L"middle right " + part_name;
            break;
          case 3:
            result = L"middle left " + part_name;
            break;
          case 4:
            result = L"lower right " + part_name;
            break;
          case 5:
            result = L"lower left " + part_name;
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
    result = Ordinal::get(number) + L" " + part_name;
  }

  return result;
}

bool Thing::can_have_action_done_by(ThingId thing, Action& action)
{
  return call_lua_function<bool>("can_have_action_" + action.get_type() + "_done_by", { thing }, false);
}

bool Thing::is_miscible_with(ThingId thing)
{
  return call_lua_function<bool>("is_miscible_with", { thing }, false);
}

BodyPart Thing::is_equippable_on() const
{
  return BodyPart::Count;
}

bool Thing::process()
{
  // Get a copy of the inventory's list of things.
  // This is because things can be deleted/removed from the inventory
  // over the course of processing them, and this might screw up the
  // iterator.
  auto things = m_inventory.get_things();

  // Process inventory.
  for (auto iter = std::begin(things);
       iter != std::end(things);
       ++iter)
  {
    ThingId thing = iter->second;
    /* bool dead = */ thing->process();
  }

  // Process self last.
  return _process_self();
}

ActionResult Thing::perform_action_died()
{
  ActionResult result = call_lua_function<ActionResult>("perform_action_died", {}, ActionResult::Success);
  return result;
}

void Thing::perform_action_collided_with(ThingId actor)
{
  /* ActionResult result = */ call_lua_function<ActionResult, ThingId>("perform_action_collided_with", { actor }, ActionResult::Success);
  return;
}

void Thing::perform_action_collided_with_wall(Direction d, StringKey tile_type)
{
  /// @todo Implement me; right now there's no way to pass one enum and one string to a Lua function.
  return;
}

ActionResult Thing::be_object_of(Action& action, ThingId subject)
{
  ActionResult result = call_lua_function<ActionResult, ThingId>("be_object_of_action_" + action.get_type(), { subject }, ActionResult::Success);
  return result;
}

ActionResult Thing::be_object_of(Action & action, ThingId subject, ThingId target)
{
  ActionResult result = call_lua_function<ActionResult, ThingId>("be_object_of_action_" + action.get_type(), { subject, target }, ActionResult::Success);
  return result;
}

ActionResult Thing::be_object_of(Action & action, ThingId subject, Direction direction)
{
  ActionResult result = call_lua_function<ActionResult>("be_object_of_action_" + action.get_type(), { subject, NULL, direction.x(), direction.y(), direction.z() }, ActionResult::Success);
  return result;
}

ActionResult Thing::perform_action_hurt_by(ThingId subject)
{
  ActionResult result = call_lua_function<ActionResult, ThingId>("be_object_of_action_hurt", { subject }, ActionResult::Success);
  return result;
}

ActionResult Thing::perform_action_attacked_by(ThingId subject, ThingId target)
{
  ActionResult result = call_lua_function<ActionResult, ThingId>("be_object_of_action_attack", { subject, target }, ActionResult::Success);
  return result;
}

bool Thing::perform_action_deequipped_by(ThingId actor, WearLocation& location)
{
  if (this->get_modified_property<bool>("bound"))
  {
    StringDisplay message;
    message = actor->get_identifying_string() + L" cannot take off " + this->get_identifying_string() +
      L"; it is magically bound to " +
      actor->get_possessive_adjective() + L" " +
      actor->get_bodypart_description(location.part,
                                      location.number) + L"!";
    the_message_log.add(message);
    return false;
  }
  else
  {
    ActionResult result = call_lua_function<ActionResult, ThingId>("perform_action_deequipped_by", { actor }, ActionResult::Success);
    return was_successful(result);
  }
}

bool Thing::perform_action_equipped_by(ThingId actor, WearLocation& location)
{
  ActionResult result = call_lua_function<ActionResult, ThingId>("perform_action_equipped_by", { actor }, ActionResult::Success);
  bool subclass_result = was_successful(result);

  return subclass_result;
}

bool Thing::perform_action_unwielded_by(ThingId actor)
{
  ActionResult result = call_lua_function<ActionResult, ThingId>("perform_action_unwielded_by", { actor }, ActionResult::Success);
  return was_successful(result);
}

bool Thing::can_merge_with(ThingId other) const
{
  // Things with different types can't merge (obviously).
  if (other->get_type() != get_type())
  {
    return false;
  }

  // Things with inventories can never merge.
  if ((get_intrinsic<int>("inventory_size") != 0) ||
    (other->get_intrinsic<int>("inventory_size") != 0))
  {
    return false;
  }

  // If the things have the exact same properties, merge is okay.
  /// @todo Handle default properties. Right now, if a property was
  ///       queried on a Thing and pulls the default, but it was NOT queried
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

ActionResult Thing::can_contain(ThingId thing)
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
    return call_lua_function<ActionResult, ThingId>("can_contain", { thing }, ActionResult::Success);
  }
}

void Thing::set_location(ThingId target)
{
  m_location = target;
}

// *** PROTECTED METHODS ******************************************************

bool Thing::_process_self()
{
  int counter_busy = get_base_property<int>("counter_busy");

  // Is this an entity that is now dead?
  if (is_subtype_of("Entity") && (get_modified_property<int>("hp") <= 0))
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
      CLOG(TRACE, "Thing") << "Thing " <<
        get_id() << "( " <<
        get_type() << "): Action " <<
        action->get_type() << " is done, popping";

      m_pending_actions.pop_front();
    }
  } // end if (actions pending)
  // Otherwise if there are no other pending actions...
  else
  {
    // If entity is not the player, call the Lua process function on this Thing.
    /// @todo Should this be called regardless of whether actions are pending
    ///       or not? In other words, should a Thing be able to "change its mind"
    ///       and clear out any pending actions?
    if (!is_player())
    {
      ActionResult result = call_lua_function<ActionResult>("process");
    }
  }

  return true;
}

std::vector<StringKey>& Thing::get_map_memory()
{
  return m_map_memory;
}

void Thing::do_recursive_visibility(int octant,
                                    int depth,
                                    float slope_A,
                                    float slope_B)
{
  sf::Vector2i new_coords;
  //int x = 0;
  //int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (is_inside_another_thing())
  {
    return;
  }

  MapTile* tile = get_maptile();
  sf::Vector2i tile_coords = tile->get_coords();
  Map& game_map = GAME.get_maps().get(get_map_id());

  static const int mv = 128;
  static constexpr int mw = (mv * mv);

  std::function< bool(sf::Vector2f, sf::Vector2f, float) > loop_condition;
  Direction dir;
  std::function< float(sf::Vector2f, sf::Vector2f) > recurse_slope;
  std::function< float(sf::Vector2f, sf::Vector2f) > loop_slope;

  switch (octant)
  {
    case 1:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::West;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + Direction::Northwest.half(), b); };
      break;

    case 2:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y - depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::East;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_slope(a + Direction::Northeast.half(), b); };
      break;

    case 3:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::North;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_inv_slope(a + Direction::Northeast.half(), b); };
      break;

    case 4:
      new_coords.x = tile_coords.x + depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::South;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + Direction::Southwest.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      break;

    case 5:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) + (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) >= c; };
      dir = Direction::East;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + Direction::Southeast.half(), b); };
      break;

    case 6:
      new_coords.x = static_cast<int>(rint(static_cast<float>(tile_coords.x) - (slope_A * static_cast<float>(depth))));
      new_coords.y = tile_coords.y + depth;
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_slope(a, b) <= c; };
      dir = Direction::West;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_slope(a + Direction::Northwest.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_slope(a + Direction::Southwest.half(), b); };
      break;

    case 7:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) + (slope_A * static_cast<float>(depth))));
      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) <= c; };
      dir = Direction::South;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + Direction::Southeast.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return -calc_inv_slope(a + Direction::Southwest.half(), b); };
      break;

    case 8:
      new_coords.x = tile_coords.x - depth;
      new_coords.y = static_cast<int>(rint(static_cast<float>(tile_coords.y) - (slope_A * static_cast<float>(depth))));

      loop_condition = [](sf::Vector2f a, sf::Vector2f b, float c) { return calc_inv_slope(a, b) >= c; };
      dir = Direction::North;
      recurse_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + Direction::Northeast.half(), b); };
      loop_slope = [](sf::Vector2f a, sf::Vector2f b) { return calc_inv_slope(a + Direction::Northwest.half(), b); };
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
        if (!game_map.tile_is_opaque(new_coords + (sf::Vector2i)dir))
        {
          do_recursive_visibility(octant, depth + 1, slope_A, recurse_slope(to_v2f(new_coords), to_v2f(tile_coords)));
        }
      }
      else
      {
        if (game_map.tile_is_opaque(new_coords + (sf::Vector2i)dir))
        {
          slope_A = loop_slope(to_v2f(new_coords), to_v2f(tile_coords));
        }
      }
      m_tiles_currently_seen[game_map.get_index(new_coords.x, new_coords.y)] = true;
      m_map_memory[game_map.get_index(new_coords.x, new_coords.y)] = game_map.get_tile(new_coords.x,
                                                                                            new_coords.y).get_tile_type();
    }
    new_coords -= (sf::Vector2i)dir;
  }
  new_coords += (sf::Vector2i)dir;

  if ((depth < mv) && (!game_map.get_tile(new_coords.x, new_coords.y).is_opaque()))
  {
    do_recursive_visibility(octant, depth + 1, slope_A, slope_B);
  }
}

MapTile* Thing::_get_maptile() const
{
  return m_map_tile;
}