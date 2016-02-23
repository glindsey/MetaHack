#include "Thing.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>

#include <cinttypes>
#include <iomanip>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

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

// Local definitions to make reading/writing status info a bit easier.
#define YOU       (this->get_you_or_identifying_string())  // "you" or descriptive noun like "the goblin"
#define YOU_SUBJ  (this->get_subject_pronoun())     // "you/he/she/it/etc."
#define YOU_OBJ   (this->get_object_pronoun())      // "you/him/her/it/etc."
#define YOUR      (this->get_possessive())          // "your/his/her/its/etc."
#define YOURSELF  (this->get_reflexive_pronoun())   // "yourself/himself/herself/itself/etc."

#define CV(p12, p3)  (this->choose_verb(p12, p3))   // shortcut for "Choose Verb"

#define ARE   (this->choose_verb(" are", " is"))
#define WERE  (this->choose_verb(" were", " was"))
#define DO    (this->choose_verb(" do", " does"))
#define GET   (this->choose_verb(" get", " gets"))
#define HAVE  (this->choose_verb(" have", " has"))
#define SEEM  (this->choose_verb(" seem", " seems"))
#define TRY   (this->choose_verb(" try", " tries"))

#define IS_PLAYER (GAME.get_player() == pImpl->ref)

#define FOO       (thing->get_identifying_string())
#define FOO1      (thing1->get_identifying_string())
#define FOO2      (thing2->get_identifying_string())
#define LIQUID1      (liquid1->get_identifying_string())
#define LIQUID2      (liquid2->get_identifying_string())

#define YOU_ARE       (YOU + ARE)
#define YOU_WERE      (YOU + WERE)
#define YOU_SUBJ_ARE  (YOU_SUBJ + ARE)
#define YOU_DO        (YOU + DO)
#define YOU_GET       (YOU + GET)
#define YOU_HAVE      (YOU + HAVE)
#define YOU_SEEM      (YOU + SEEM)
#define YOU_TRY       (YOU + TRY)

// Static member initialization.
sf::Color const Thing::wall_outline_color_ = sf::Color(255, 255, 255, 64);

Thing::Thing(Metadata& metadata, ThingRef ref)
  : pImpl(metadata, ref)
{
  initialize();
}

Thing::Thing(MapTile* map_tile, Metadata& metadata, ThingRef ref)
  : pImpl(map_tile, metadata, ref)
{
  initialize();
}

Thing::Thing(Thing const& original, ThingRef ref)
  : pImpl(*(original.pImpl), ref)
{
  initialize();
}

void Thing::initialize()
{
  /// Get the max_hp IntegerRange for this Thing (if any).
  IntegerRange max_hp_range = pImpl->metadata.get_default<IntegerRange>("maxhp");

  /// Pick a number and set it as our maximum HP.
  int max_hp = max_hp_range.pick();
  set_property<int>("maxhp", max_hp);

  /// Also set our HP to that value.
  set_property<int>("hp", max_hp);
}

Thing::~Thing()
{
}

void Thing::queue_action(std::unique_ptr<Action> pAction)
{
  pImpl->pending_actions.push_back(std::move(pAction));
}

bool Thing::action_is_pending() const
{
  return !(pImpl->pending_actions.empty());
}

bool Thing::action_is_in_progress()
{
  return (get_property<int>("counter_busy") > 0);
}

ThingRef Thing::get_wielding(unsigned int& hand)
{
  return pImpl->wielding_in(hand);
}

bool Thing::is_wielding(ThingRef thing)
{
  unsigned int dummy;
  return is_wielding(thing, dummy);
}

bool Thing::is_wielding(ThingRef thing, unsigned int& hand)
{
  return pImpl->is_wielding(thing, hand);
}

bool Thing::has_equipped(ThingRef thing)
{
  WearLocation dummy;
  return has_equipped(thing, dummy);
}

bool Thing::has_equipped(ThingRef thing, WearLocation& location)
{
  return pImpl->is_wearing(thing, location);
}

bool Thing::can_reach(ThingRef thing)
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

bool Thing::is_adjacent_to(ThingRef thing)
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
  std::string message;

  switch (result)
  {
    case ActionResult::Success:
      if (this->is_player())
      {
        message = YOU + " die...";
        the_message_log.add(message);
      }
      else
      {
        bool living = get_property<bool>("living");
        if (living)
        {
          message = YOU_ARE + " killed!";
        }
        else
        {
          message = YOU_ARE + " destroyed!";
        }
        the_message_log.add(message);
      }

      // Set the property saying the entity is dead.
      set_property<bool>("dead", true);

      // Clear any pending actions.
      pImpl->pending_actions.clear();

      return true;
    case ActionResult::Failure:
    default:
      message = YOU + CV(" manage", " manages") + " to avoid dying.";
      the_message_log.add(message);
      return false;
  }
}

bool Thing::do_attack(ThingRef thing, unsigned int& action_time)
{
  std::string message;

  bool reachable = this->is_adjacent_to(thing);
  /// @todo deal with Entities in your Inventory -- WTF do you do THEN?

  if (reachable)
  {
    /// @todo Write attack code.
    message = YOU_TRY + " to attack " + FOO + ", but are stopped by the programmer's procrastination!";
    the_message_log.add(message);
  }

  return false;
}

ActionResult Thing::can_deequip(ThingRef thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing == pImpl->ref)
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

bool Thing::do_deequip(ThingRef thing, unsigned int& action_time)
{
  std::string message;
  ActionResult deequip_try = this->can_deequip(thing, action_time);
  std::string thing_name = thing->get_identifying_string();

  message = YOU_TRY + " to take off " + thing_name;
  the_message_log.add(message);

  switch (deequip_try)
  {
    case ActionResult::Success:
    {
      // Get the body part this item is equipped on.
      WearLocation location;
      this->has_equipped(thing, location);

      if (thing->perform_action_deequipped_by(pImpl->ref, location))
      {
        set_worn(MU, location);

        std::string wear_desc = get_bodypart_description(location.part, location.number);
        message = YOU_ARE + " no longer wearing " + thing_name +
          " on " + YOUR + " " + wear_desc + ".";
        the_message_log.add(message);
        return true;
      }
    }
    break;

    case ActionResult::FailureItemNotEquipped:
    {
      message = YOU_ARE + " not wearing " + thing_name + ".";
      the_message_log.add(message);
      return true;
    }
    break;

    default:
      MINOR_ERROR("Unknown ActionResult %d", deequip_try);
      break;
  }

  return false;
}

ActionResult Thing::can_equip(ThingRef thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing == pImpl->ref)
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

bool Thing::do_equip(ThingRef thing, unsigned int& action_time)
{
  std::string message;

  ActionResult equip_try = this->can_equip(thing, action_time);
  std::string thing_name = thing->get_identifying_string();

  switch (equip_try)
  {
    case ActionResult::Success:
    {
      WearLocation location;

      if (thing->perform_action_equipped_by(pImpl->ref, location))
      {
        set_worn(thing, location);

        std::string wear_desc = get_bodypart_description(location.part,
                                                         location.number);
        message = YOU_ARE + " now wearing " + thing_name +
          " on " + YOUR + " " + wear_desc + ".";
        the_message_log.add(message);
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
        message = YOU_TRY + " to equip " + YOURSELF +
          ", which seriously shouldn't happen.";
        MINOR_ERROR("NPC tried to equip self!?");
      }
      the_message_log.add(message);
      break;

    case ActionResult::FailureThingOutOfReach:
    {
      message = YOU_TRY + " to equip " + thing_name + ".";
      the_message_log.add(message);

      message = thing_name + " is not in " + YOUR + " inventory.";
      the_message_log.add(message);
    }
    break;

    case ActionResult::FailureItemNotEquippable:
    {
      message = YOU_TRY + " to equip " + thing_name + ".";
      the_message_log.add(message);

      message = thing_name + " is not an equippable item.";
      the_message_log.add(message);
    }
    break;

    default:
      MINOR_ERROR("Unknown ActionResult %d", equip_try);
      break;
  }

  return false;
}

void Thing::set_wielded(ThingRef thing, unsigned int hand)
{
  if (thing == MU)
  {
    pImpl->wielded_items.erase(hand);
  }
  else
  {
    pImpl->wielded_items[hand] = thing;
  }
}

void Thing::set_worn(ThingRef thing, WearLocation location)
{
  if (thing == MU)
  {
    pImpl->equipped_items.erase(location);
  }
  else
  {
    pImpl->equipped_items[location] = thing;
  }
}

bool Thing::can_currently_see()
{
  return get_intrinsic<bool>("can_see", false) && (get_property<int>("counters.blind", 0) == 0);
}

bool Thing::can_currently_move()
{
  return get_intrinsic<bool>("can_move", false) && (get_property<int>("counters.paralyzed", 0) == 0);
}

void Thing::set_gender(Gender gender)
{
  pImpl->gender = gender;
}

Gender Thing::get_gender() const
{
  return pImpl->gender;
}

Gender Thing::get_gender_or_you() const
{
  if (is_player())
  {
    return Gender::SecondPerson;
  }
  else
  {
    return pImpl->gender;
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
std::string Thing::get_bodypart_name(BodyPart part) const
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

/// Get the appropriate body part plural for the Entity.
std::string Thing::get_bodypart_plural(BodyPart part) const
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

bool Thing::is_player() const
{
  return (GAME.get_player() == pImpl->ref);
}

std::string const& Thing::get_type() const
{
  return pImpl->metadata.get_type();
}

std::string const& Thing::get_parent_type() const
{
  return pImpl->metadata.get_intrinsic<std::string>("parent");
}

unsigned int Thing::get_quantity()
{
  return get_property<unsigned int>("quantity", 1);
}

void Thing::set_quantity(unsigned int quantity)
{
  set_property<unsigned int>("quantity", quantity);
}

ThingRef Thing::get_ref() const
{
  return pImpl->ref;
}

ThingRef Thing::get_root_location() const
{
  if (pImpl->location == MU)
  {
    return pImpl->ref;
  }
  else
  {
    auto location = pImpl->location;
    return location->get_root_location();
  }
}

ThingRef Thing::get_location() const
{
  return pImpl->location;
}

bool Thing::can_see(ThingRef thing)
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

  Map& game_map = GAME.get_map_factory().get(map_id);

  if (can_currently_see())
  {
    // Return seen data.
    return pImpl->tiles_currently_seen[game_map.get_index(xTile, yTile)];
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
  ThingRef location = get_location();
  if (location == MU)
  {
    return;
  }

  // Clear the "tile seen" bitset.
  pImpl->tiles_currently_seen.reset();

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

  //TRACE("find_seen_tiles took %d ms", elapsed.getElapsedTime().asMilliseconds());
}

std::string Thing::get_memory_at(int x, int y) const
{
  if (this->get_map_id() == MapFactory::null_map_id)
  {
    return "???";
  }

  Map& game_map = GAME.get_map_factory().get(this->get_map_id());
  return pImpl->map_memory[game_map.get_index(x, y)];
}

std::string Thing::get_memory_at(sf::Vector2i coords) const
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
  Map& game_map = GAME.get_map_factory().get(map_id);

  static sf::Vertex new_vertex;
  float ts = Settings.get<float>("map_tile_size");
  float ts2 = ts * 0.5f;

  sf::Vector2f location(x * ts, y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);

  std::string tile_type = pImpl->map_memory[game_map.get_index(x, y)];
  if (tile_type == "") { tile_type = "MTUnknown"; }
  Metadata* tile_metadata = &(MDC::get_collection("maptile").get(tile_type));

  /// @todo Call a script to handle selecting a tile other than the one
  ///       in the upper-left corner.
  sf::Vector2u tile_coords = tile_metadata->get_tile_coords();

  TileSheet::add_quad(vertices,
                      tile_coords, sf::Color::White,
                      vNW, vNE,
                      vSW, vSE);
}

bool Thing::move_into(ThingRef new_location)
{
  MapId old_map_id = this->get_map_id();
  ThingRef old_location = pImpl->location;

  if (new_location == old_location)
  {
    // We're already there!
    return true;
  }

  ActionResult can_contain = new_location->can_contain(pImpl->ref);

  switch (can_contain)
  {
    case ActionResult::Success:
      if (new_location->get_inventory().add(pImpl->ref))
      {
        // Try to lock our old location.
        if (pImpl->location != MU)
        {
          pImpl->location->get_inventory().remove(pImpl->ref);
        }

        // Set the location to the new location.
        pImpl->location = new_location;

        MapId new_map_id = this->get_map_id();
        if (old_map_id != new_map_id)
        {
          if (old_map_id != MapFactory::null_map_id)
          {
            /// @todo Save old map memory.
          }
          pImpl->map_memory.clear();
          pImpl->tiles_currently_seen.clear();
          if (new_map_id != MapFactory::null_map_id)
          {
            Map& new_map = GAME.get_map_factory().get(new_map_id);
            sf::Vector2i new_map_size = new_map.get_size();
            pImpl->map_memory.resize(new_map_size.x * new_map_size.y);
            pImpl->tiles_currently_seen.resize(new_map_size.x * new_map_size.y);
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
  return pImpl->inventory;
}

bool Thing::is_inside_another_thing() const
{
  ThingRef location = pImpl->location;
  if (location == MU)
  {
    // Thing is a part of the MapTile such as the floor.
    return false;
  }

  ThingRef location2 = location->get_location();
  if (location2 == MU)
  {
    // Thing is directly on the floor.
    return false;
  }
  return true;
}

MapTile* Thing::get_maptile() const
{
  ThingRef location = pImpl->location;

  if (location == MU)
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
  ThingRef location = pImpl->location;

  if (location == MU)
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

std::string Thing::get_display_name() const
{
  /// @todo Implement adding adjectives.
  return pImpl->metadata.get_intrinsic<std::string>("name");
}

std::string Thing::get_display_plural() const
{
  return pImpl->metadata.get_intrinsic<std::string>("plural");
}

std::string Thing::get_proper_name()
{
  return get_property<std::string>("proper_name");
}

void Thing::set_proper_name(std::string name)
{
  set_property<std::string>("proper_name", name);
}

std::string Thing::get_identifying_string_without_possessives(bool definite)
{
  ThingRef location = this->get_location();
  unsigned int quantity = this->get_quantity();

  std::string name;

  std::string article;
  std::string adjectives;
  std::string noun;
  std::string suffix;

  // If the thing is YOU, use YOU.
  if (is_player())
  {
    if (get_property<int>("hp") > 0)
    {
      return "you";
    }
    else
    {
      return "your corpse";
    }
  }

  if (quantity == 1)
  {
    noun = get_display_name();

    if (definite)
    {
      article = "the ";
    }
    else
    {
      article = getIndefArt(noun) + " ";
    }

    if (get_proper_name().empty() == false)
    {
      suffix = " named " + get_proper_name();
    }
  }
  else
  {
    noun = get_display_plural();

    if (definite)
    {
      article = "the ";
    }
    article += boost::lexical_cast<std::string>(get_quantity()) + " ";
  }

  if (get_intrinsic<bool>("is_entity") && get_property<int>("hp") <= 0)
  {
    adjectives += "dead ";
  }

  name = article + adjectives + noun + suffix;

  return name;
}

std::string Thing::get_you_or_identifying_string(bool definite)
{
  // If the thing is YOU, use YOU.
  if (is_player())
  {
    if (get_property<int>("hp") > 0)
    {
      return "you";
    }
    else
    {
      return "your corpse";
    }
  }

  return get_identifying_string(definite);
}

std::string Thing::get_self_or_identifying_string(ThingRef other, bool definite)
{
  if (other == get_ref())
  {
    return get_reflexive_pronoun();
  }

  return get_identifying_string(definite);
}

std::string Thing::get_identifying_string(bool definite)
{
  ThingRef location = this->get_location();
  unsigned int quantity = this->get_quantity();

  std::string name;

  bool owned;

  std::string article;
  std::string adjectives;
  std::string noun;
  std::string suffix;

  owned = location->get_intrinsic<bool>("is_entity");

  if (quantity == 1)
  {
    noun = get_display_name();

    if (owned)
    {
      article = location->get_possessive() + " ";
    }
    else
    {
      if (definite)
      {
        article = "the ";
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

    if (owned)
    {
      article = location->get_possessive() + " ";
    }
    else
    {
      if (definite)
      {
        article = "the ";
      }

      article += boost::lexical_cast<std::string>(get_quantity()) + " ";
    }
  }

  if (get_intrinsic<bool>("is_entity") && get_property<int>("hp") <= 0)
  {
    adjectives += "dead ";
  }

  name = article + adjectives + noun + suffix;

  return name;
}

std::string const& Thing::choose_verb(std::string const& verb12,
                                      std::string const& verb3)
{
  if ((GAME.get_player() == pImpl->ref) || (get_property<unsigned int>("quantity") > 1))
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
  return get_intrinsic<int>("physical_mass") * get_property<unsigned int>("quantity");
}

std::string const& Thing::get_subject_pronoun() const
{
  return getSubjPro(get_gender_or_you());
}

std::string const& Thing::get_object_pronoun() const
{
  return getObjPro(get_gender_or_you());
}

std::string const& Thing::get_reflexive_pronoun() const
{
  return getRefPro(get_gender_or_you());
}

std::string const& Thing::get_possessive_adjective() const
{
  return getPossAdj(get_gender_or_you());
}

std::string const& Thing::get_possessive_pronoun() const
{
  return getPossPro(get_gender_or_you());
}

std::string Thing::get_possessive()
{
  static std::string const your = std::string("your");

  if (GAME.get_player() == pImpl->ref)
  {
    return your;
  }
  else
  {
    return get_identifying_string_without_possessives(true) + "'s";
  }
}

sf::Vector2u Thing::get_tile_sheet_coords(int frame)
{
  /// Get tile coordinates on the sheet.
  sf::Vector2u start_coords = pImpl->metadata.get_tile_coords();

  /// Call the Lua function to get the offset (tile to choose).
  sf::Vector2u offset = call_lua_function_v2u("get_tile_offset", { frame });

  /// Add them to get the resulting coordinates.
  sf::Vector2u tile_coords = start_coords + offset;

  return tile_coords;
}

void Thing::add_floor_vertices_to(sf::VertexArray& vertices,
                                  bool use_lighting,
                                  int frame)
{
  sf::Vertex new_vertex;
  float ts = Settings.get<float>("map_tile_size");
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
    target_size = Settings.get<unsigned int>("map_tile_size");
  }

  unsigned int tile_size = Settings.get<unsigned int>("map_tile_size");

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
  rectangle.setTexture(&(TS.getTexture()));
  rectangle.setTextureRect(texture_coords);
  rectangle.setFillColor(thing_color);

  target.draw(rectangle);
}

bool Thing::is_opaque() const
{
  return
    (get_intrinsic<int>("opacity_red") >= 255) &&
    (get_intrinsic<int>("opacity_green") >= 255) &&
    (get_intrinsic<int>("opacity_blue") >= 255);
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
    if (!is_opaque() || get_intrinsic<bool>("is_entity"))
    {
      auto& things = get_inventory().get_things();
      for (auto& thing_pair : things)
      {
        ThingRef thing = thing_pair.second;
        thing->light_up_surroundings();
      }
    }
  }

  ThingRef location = get_location();

  // Use visitor pattern.
  if ((location != MU) && this->get_property<bool>("light_lit"))
  {
    location->be_lit_by(this->get_ref());
  }
}

void Thing::be_lit_by(ThingRef light)
{
  call_lua_function("on_lit_by", { light });

  if (get_location() == MU)
  {
    GAME.get_map_factory().get(get_map_id()).add_light(light);
  }

  /// @todo Figure out how we want to handle light sources.
  ///       If we want to be more accurate, the light should only
  ///       be able to be seen when a character is wielding or has
  ///       equipped it. If we want to be easier, the light should
  ///       shine simply if it's in the player's inventory.

  //if (!is_opaque() || is_wielding(light) || has_equipped(light))
  if (!is_opaque() || get_intrinsic<bool>("is_entity"))
  {
    ThingRef location = get_location();
    if (location != MU)
    {
      location->be_lit_by(light);
    }
  }
}

void Thing::spill()
{
  Inventory& inventory = get_inventory();
  ThingMap const& things = inventory.get_things();
  std::string message;
  bool success = false;

  // Step through all contents of this Thing.
  for (ThingPair thing_pair : things)
  {
    ThingRef thing = thing_pair.second;
    if (pImpl->location != MU)
    {
      ActionResult can_contain = pImpl->location->can_contain(thing);

      switch (can_contain)
      {
        case ActionResult::Success:

          // Try to move this into the Thing's location.
          success = thing->move_into(pImpl->location);
          if (success)
          {
            auto container_string = this->get_identifying_string();
            auto thing_string = thing->get_identifying_string();
            message = thing_string + CV(" tumble", " tumbles") + " out of " + container_string + ".";
            the_message_log.add(message);
          }
          else
          {
            // We couldn't move it, so just destroy it.
            auto container_string = this->get_identifying_string();
            auto thing_string = thing->get_identifying_string();
            message = thing_string + CV(" vanish", " vanishes") + " in a puff of logic.";
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
  auto old_location = pImpl->location;

  if (get_intrinsic<int>("inventory_size") != 0)
  {
    // Spill the contents of this Thing into the Thing's location.
    spill();
  }

  if (old_location != MU)
  {
    old_location->get_inventory().remove(pImpl->ref);
  }
}

std::string Thing::get_bodypart_description(BodyPart part,
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
      MINOR_ERROR("Request for description of %s!?", result.c_str());
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

bool Thing::can_have_action_done_by(ThingRef thing, Action& action)
{
  return call_lua_function_bool("can_have_action_" + action.get_type() + "_done_by", { thing }, false);
}

bool Thing::is_miscible_with(ThingRef thing)
{
  return call_lua_function_bool("is_miscible_with", { thing }, false);
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
  auto things = pImpl->inventory.get_things();

  // Process inventory.
  for (auto iter = std::begin(things);
  iter != std::end(things);
    ++iter)
  {
    ThingRef thing = iter->second;
    /* bool dead = */ thing->process();
  }

  // Process self last.
  return _process_self();
}

ActionResult Thing::perform_action_died()
{
  ActionResult result = call_lua_function("perform_action_died", {});
  return result;
}

bool Thing::perform_action_activated_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_activated_by", { actor });
  return was_successful(result);
}

void Thing::perform_action_collided_with(ThingRef actor)
{
  /* ActionResult result = */ call_lua_function("perform_action_collided_with", { actor });
  return;
}

void Thing::perform_action_collided_with_wall(Direction d, std::string tile_type)
{
  /// @todo Implement me; right now there's no way to pass one enum and one string to a Lua function.
  return;
}

ActionResult Thing::perform_action_drank_by(ThingRef actor)
{
  ThingRef contents = actor->get_inventory().get(INVSLOT_ZERO);
  ActionResult result = call_lua_function("perform_action_drank_by", { actor, contents });
  return result;
}

bool Thing::perform_action_dropped_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_dropped_by", { actor });
  return was_successful(result);
}

ActionResult Thing::perform_action_eaten_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_eaten_by", { actor });
  return result;
}

ActionResult Thing::perform_action_used_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_used_by", { actor });

  return result;
}

bool Thing::perform_action_picked_up_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_picked_up_by", { actor });
  return was_successful(result);
}

bool Thing::perform_action_put_into_by(ThingRef container, ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_put_into", { container, actor });
  return was_successful(result);
}

bool Thing::perform_action_taken_out_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_taken_out_by", { actor });
  return was_successful(result);
}

ActionResult Thing::perform_action_read_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_read_by", { actor });
  return result;
}

void Thing::perform_action_attack_hits(ThingRef target)
{
  /* ActionResult result = */ call_lua_function("perform_action_read_by", { target });
  return;
}

bool Thing::perform_action_thrown_by(ThingRef actor, Direction direction)
{
  ActionResult result = call_lua_function("perform_action_thrown_by", { actor, direction.x(), direction.y(), direction.z() });
  return was_successful(result);
}

bool Thing::perform_action_deequipped_by(ThingRef actor, WearLocation& location)
{
  if (this->get_property<bool>("bound"))
  {
    std::string message;
    message = actor->get_identifying_string() + " cannot take off " + this->get_identifying_string() +
      "; it is magically bound to " +
      actor->get_possessive_adjective() + " " +
      actor->get_bodypart_description(location.part,
                                      location.number) + "!";
    the_message_log.add(message);
    return false;
  }
  else
  {
    ActionResult result = call_lua_function("perform_action_deequipped_by", { actor });
    return was_successful(result);
  }
}

bool Thing::perform_action_equipped_by(ThingRef actor, WearLocation& location)
{
  ActionResult result = call_lua_function("perform_action_equipped_by", { actor });
  bool subclass_result = was_successful(result);

  if (subclass_result == true)
  {
    if (this->get_property<bool>("autobinds"))
    {
      this->set_property<bool>("bound", true);
      std::string message;
      message = this->get_identifying_string() + " magically binds itself to " +
        actor->get_possessive() + " " +
        actor->get_bodypart_description(location.part,
                                        location.number) + "!";
      the_message_log.add(message);
    }
  }

  return subclass_result;
}

bool Thing::perform_action_unwielded_by(ThingRef actor)
{
  if (this->get_property<bool>("bound"))
  {
    std::string message;
    message = actor->get_identifying_string() + " cannot unwield " + this->get_identifying_string() +
      "; it is magically bound to " +
      actor->get_possessive_adjective() + " " +
      actor->get_bodypart_name(BodyPart::Hand) + "!";
    the_message_log.add(message);
    return false;
  }
  else
  {
    ActionResult result = call_lua_function("perform_action_unwielded_by", { actor });
    return was_successful(result);
  }
}

bool Thing::perform_action_wielded_by(ThingRef actor)
{
  ActionResult result = call_lua_function("perform_action_wielded_by", { actor });
  bool subclass_result = was_successful(result);

  if (subclass_result == true)
  {
    if (this->get_property<bool>("autobinds"))
    {
      this->set_property<bool>("bound", true);
      std::string message;
      message = this->get_identifying_string() + " magically binds itself to " +
        actor->get_possessive() + " " +
        actor->get_bodypart_name(BodyPart::Hand) + "!";
      the_message_log.add(message);
    }
  }

  return subclass_result;
}

bool Thing::perform_action_fired_by(ThingRef actor, Direction direction)
{
  ActionResult result = call_lua_function("perform_action_fired_by", { actor, direction.x(), direction.y(), direction.z() });
  return was_successful(result);
}

bool Thing::can_merge_with(ThingRef other) const
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

  // If the things have the same properties, merge is okay.
  /// @todo Implement this. It's going to be a major pain in the ass.
  ///       We'll need a Lua function that compares the property arrays
  ///       of the two Things, and returns true or false.
  //if ((this->pImpl->properties) == (other->pImpl->properties))
  {
    return true;
  }

  return false;
}

ActionResult Thing::can_contain(ThingRef thing)
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
    return call_lua_function("can_contain", { thing });
  }
}

void Thing::set_location(ThingRef target)
{
  pImpl->location = target;
}

// *** PROTECTED METHODS ******************************************************

bool Thing::_process_self()
{
  int counter_busy = get_property<int>("counter_busy");

  // Is this an entity that is now dead?
  if ((get_property<bool>("is_entity") == true) && (get_property<int>("hp") <= 0))
  {
    // Did the entity JUST die?
    if (get_property<bool>("dead") != true)
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
    add_to_property<int>("counter_busy", -1);
  }
  // Otherwise if actions are pending...
  else if (!pImpl->pending_actions.empty())
  {
    // Process the front action.
    std::unique_ptr<Action>& action = pImpl->pending_actions.front();
    bool action_done = action->process(get_ref(), {});
    if (action_done)
    {
      TRACE("Thing %s (%s): Action %s is done, popping",
            get_ref().get_id().to_string().c_str(),
            get_type().c_str(),
            action->get_type().c_str());
      pImpl->pending_actions.pop_front();
    }
  } // end if (actions pending)

  return true;
}

std::vector<std::string>& Thing::get_map_memory()
{
  return pImpl->map_memory;
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
  Map& game_map = GAME.get_map_factory().get(get_map_id());

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
      pImpl->tiles_currently_seen[game_map.get_index(new_coords.x, new_coords.y)] = true;
      pImpl->map_memory[game_map.get_index(new_coords.x, new_coords.y)] = game_map.get_tile(new_coords.x,
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

// *** PRIVATE METHODS ********************************************************

MapTile* Thing::_get_maptile() const
{
  return pImpl->map_tile;
}

ActionResult Thing::call_lua_function(std::string function_name, std::vector<lua_Integer> const& args,
                                      ActionResult default_result)
{
  return pImpl->metadata.call_lua_function(function_name, get_ref(), args, default_result);
}

bool Thing::call_lua_function_bool(std::string function_name, std::vector<lua_Integer> const& args, bool default_result)
{
  return pImpl->metadata.call_lua_function_bool(function_name, get_ref(), args, default_result);
}

sf::Vector2u Thing::call_lua_function_v2u(std::string function_name, std::vector<lua_Integer> const& args,
                                          sf::Vector2u default_result)
{
  return pImpl->metadata.call_lua_function_v2u(function_name, get_ref(), args, default_result);
}