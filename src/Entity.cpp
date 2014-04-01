#include "Entity.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <memory>
#include <sstream>

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "MessageLog.h"
#include "Ordinal.h"
#include "Thing.h"
#include "ThingFactory.h"
#include "TileSheet.h"

#include "ai/AIStrategy.h"

// Local definitions to make reading/writing status info a bit easier.
#define _YOU_   (this->get_name())
#define _ARE_   (this->choose_verb(" are", " is"))
#define _DO_    (this->choose_verb(" do", " does"))
#define _YOUR_  (this->get_possessive())
#define _HAVE_  (this->choose_verb(" have", " has"))
#define _YOURSELF_ (this->get_reflexive_pronoun())
#define _TRY_   (this->choose_verb(" try", " tries"))

#define _YOU_ARE_ _YOU_ + _ARE_
#define _YOU_DO_ _YOU_ + _DO_
#define _YOU_TRY_ _YOU_ + _TRY_

// Using declarations.
using WieldingMap = std::map<unsigned int, std::weak_ptr<Thing>>;
using WieldingPair = std::pair<unsigned int, std::weak_ptr<Thing>>;

using WearingMap = std::map<WearLocation, std::weak_ptr<Thing>>;
using WearingPair = std::pair<WearLocation, std::weak_ptr<Thing>>;

struct Entity::Impl
{
  /// Entity's attributes.
  AttributeSet attributes;

  /// Number of game cycles until this Entity can process another command.
  int busy_counter = 0;

  /// Gender of this entity.
  Gender gender = Gender::None;

  /// Entity's memory of map tiles.
  std::vector<MapTileType> map_memory;

  /// Bitset for seen tiles.
  boost::dynamic_bitset<> tile_seen;

  /// @todo Blind counter.

  /// AI strategy associated with this Entity (if any).
  std::shared_ptr<AIStrategy> ai_strategy;

  /// Queue of actions to be performed.
  std::deque<Action> actions;

  /// Map of items wielded.
  WieldingMap wielded_items;

  /// Map of things worn.
  WearingMap equipped_items;

  bool is_wielding(Thing* thing, unsigned int& hand)
  {
    if (thing == nullptr)
    {
      return false;
    }
    auto found_item = std::find_if(wielded_items.cbegin(),
                                   wielded_items.cend(),
                                   [&](WieldingPair const& p)
                                   { return p.second.lock().get() == thing; });

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

  std::shared_ptr<Thing> wielding_in(unsigned int hand)
  {
    if (wielded_items.count(hand) == 0)
    {
      return std::shared_ptr<Thing>();
    }
    else
    {
      return wielded_items[hand].lock();
    }
  }

  bool is_wearing(Thing* thing, WearLocation& location)
  {
    if (thing == nullptr)
    {
      return false;
    }
    auto found_item = std::find_if(equipped_items.cbegin(),
                                   equipped_items.cend(),
                                   [&](WearingPair const& p)
                                   { return p.second.lock().get() == thing; });

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

  void wield(std::shared_ptr<Thing> thing, unsigned int hand)
  {
    wielded_items[hand] = thing;
  }

  bool unwield(Thing& thing)
  {
    unsigned int hand;
    if (is_wielding(&thing, hand) == false)
    {
      return false;
    }
    else
    {
      wielded_items.erase(hand);
      return true;
    }
  }

  bool unwield(unsigned int hand)
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

  bool equip(std::shared_ptr<Thing> thing, WearLocation location)
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

  bool deequip(Thing& thing)
  {
    WearLocation location;
    if (is_wearing(&thing, location) == false)
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

Entity::Entity() :
  Thing(-1), impl(new Impl())
{
}

Entity::Entity(const Entity& original) :
  Thing(original.get_inventory_size()), impl(new Impl())
{
  impl->attributes = original.get_attributes();
  impl->busy_counter = original.get_busy_counter();
  impl->gender = original.get_gender();
  // Memory, seen tiles, and actions are not copied over.
}

Entity::~Entity()
{
  //dtor
}

std::string Entity::get_name() const
{
  /// @todo Handle tamed entities, e.g. "Bilbo's pet cockatrice"
  return get_def_name();
}

std::string Entity::get_def_name() const
{
  // If the thing is YOU, use YOU.
  if (is_player())
  {
    if (impl->attributes.get(Attribute::HP) > 0)
    {
      return "you";
    }
    else
    {
      return "your corpse";
    }
  }

  std::string name;

  if (impl->attributes.get(Attribute::HP) > 0)
  {
    std::string description = get_description();
    name = "the " + description;

    // If the Thing has a proper name, use that.
    if (get_proper_name().empty() == false)
    {
      // e.g. "the hill orc named Thrag"
      name += " named " + get_proper_name();
    }
  }
  else
  {
    // If the Thing has a proper name, use that.
    if (get_proper_name().empty() == false)
    {
      // e.g. "Fluffy's corpse"
      name = get_possessive() + " corpse";
    }
    else
    {
      // e.g. "the dead rat"
      std::string description = get_description();
      name = "the dead " + description;
    }
  }

  return name;
}

std::string Entity::get_indef_name() const
{
  // If the thing is YOU, use YOU.
  if (is_player())
  {
    if (impl->attributes.get(Attribute::HP) > 0)
    {
      return "you";
    }
    else
    {
      return "your corpse";
    }
  }

  std::string name;

  if (impl->attributes.get(Attribute::HP) > 0)
  {
    std::string description = get_description();
    name = getIndefArt(description) + " " + description;

    // If the Thing has a proper name, use that.
    if (get_proper_name().empty() == false)
    {
      // e.g. "a hill orc named Thrag"
      name += " named " + get_proper_name();
    }
  }
  else
  {
    // If the Thing has a proper name, use that.
    if (get_proper_name().empty() == false)
    {
      // e.g. "Fluffy's corpse"
      name = get_possessive() + " corpse";
    }
    else
    {
      // e.g. "a dead rat"
      std::string description = get_description();
      name = getIndefArt(description) + " dead " + description;
    }
  }

  return name;
}

int Entity::get_busy_counter() const
{
  return impl->busy_counter;
}

void Entity::set_gender(Gender gender)
{
  impl->gender = gender;
}

Gender Entity::get_true_gender() const
{
  return impl->gender;
}

Gender Entity::get_gender() const
{
  if (is_player())
  {
    return Gender::SecondPerson;
  }
  else
  {
    return impl->gender;
  }
}

bool Entity::can_move(Direction direction)
{
  MapId game_map_id = this->get_map_id();
  if (game_map_id != MapFactory::null_map_id)
  {
    auto tile = get_maptile();
    if (tile == nullptr)
    {
      return false;
    }

    sf::Vector2i coords = tile->get_coords();
    sf::Vector2i check_coords;

    Map& game_map = MF.get(game_map_id);
    bool is_in_bounds = game_map.calc_coords(coords, direction, check_coords);

    if (is_in_bounds)
    {
      auto& new_tile = game_map.get_tile(check_coords);
      return new_tile->can_be_traversed_by(*this);
    }
  }
  return false;
}

bool Entity::move_into(std::shared_ptr<Thing> new_location)
{
  MapId old_map_id = this->get_map_id();
  bool move_okay = Thing::move_into(new_location);
  if (move_okay)
  {
    MapId new_map_id = this->get_map_id();
    if (old_map_id != new_map_id)
    {
      if (old_map_id != MapFactory::null_map_id)
      {
        /// @todo Save old map memory.
      }
      impl->map_memory.clear();
      impl->tile_seen.clear();
      if (new_map_id != MapFactory::null_map_id)
      {
        Map& new_map = MF.get(new_map_id);
        sf::Vector2i new_map_size = new_map.get_size();
        impl->map_memory.resize(new_map_size.x * new_map_size.y);
        impl->tile_seen.resize(new_map_size.x * new_map_size.y);
        /// @todo Load new map memory if it exists somewhere.
      }
    }
    this->find_seen_tiles();
  }
  return move_okay;
}

bool Entity::can_see(Thing& thing)
{
  // Are we on a map?  Bail out if we aren't.
  MapId entity_map_id = this->get_map_id();
  MapId thing_map_id = thing.get_map_id();

  if ((entity_map_id == MapFactory::null_map_id) ||
      (thing_map_id == MapFactory::null_map_id) ||
      (entity_map_id != thing_map_id))
  {
    return false;
  }

  auto thing_location = thing.get_maptile();
  if (thing_location == nullptr)
  {
    return false;
  }

  sf::Vector2i thing_coords = thing_location->get_coords();

  return can_see(thing_coords.x, thing_coords.y);
}

bool Entity::can_see(sf::Vector2i coords)
{
  return this->can_see(coords.x, coords.y);
}

bool Entity::can_see(int xTile, int yTile)
{
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

  Map& game_map = MF.get(map_id);

  if (can_currently_see())
  {
    // Return seen data.
    return impl->tile_seen[game_map.get_index(xTile, yTile)];
  }
  else
  {
    return false;
  }
}

void Entity::find_seen_tiles()
{
  sf::Clock elapsed;

  elapsed.restart();

  // Are we on a map?  Bail out if we aren't.
  auto location = get_location();
  if (!location)
  {
    return;
  }

  // Clear the "tile seen" bitset.
  impl->tile_seen.reset();

  do_recursive_visibility(1, 1, 1, 0);
  do_recursive_visibility(2, 1, 1, 0);
  do_recursive_visibility(3, 1, 1, 0);
  do_recursive_visibility(4, 1, 1, 0);
  do_recursive_visibility(5, 1, 1, 0);
  do_recursive_visibility(6, 1, 1, 0);
  do_recursive_visibility(7, 1, 1, 0);
  do_recursive_visibility(8, 1, 1, 0);

  //TRACE("find_seen_tiles took %d ms", elapsed.getElapsedTime().asMilliseconds());
}

MapTileType Entity::get_memory_at(int x, int y) const
{
  if (this->get_map_id() == MapFactory::null_map_id)
  {
    return MapTileType::Unknown;
  }

  Map& game_map = MF.get(this->get_map_id());
  return impl->map_memory[game_map.get_index(x, y)];
}

MapTileType Entity::get_memory_at(sf::Vector2i coords) const
{
  return this->get_memory_at(coords.x, coords.y);
}

void Entity::add_memory_vertices_to(sf::VertexArray& vertices,
                                    int x, int y)
{
  MapId map_id = this->get_map_id();
  if (map_id == MapFactory::null_map_id)
  {
    return;
  }
  Map& game_map = MF.get(map_id);

  static sf::Vertex new_vertex;
  float ts = static_cast<float>(Settings.map_tile_size);
  float ts2 = ts * 0.5;

  sf::Vector2f location(x * ts, y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);

  MapTileType tile_type = impl->map_memory[game_map.get_index(x, y)];
  sf::Vector2u tile_coords = getMapTileTypeTileSheetCoords(tile_type);

  TileSheet::add_quad(vertices,
                          tile_coords, sf::Color::White,
                          vNW, vNE, vSW, vSE);
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
    MINOR_ERROR("Request for description of %s!?", result.c_str());
    break;

  case 1: // only one of them
    result = part_name;
    break;

  case 2: // assume a right and left one.
    switch (number)
    {
      case 0: result = "right " + part_name; break;
      case 1: result = "left " + part_name; break;
      default: break;
    }
    break;

  case 3: // assume right, center, and left.
    switch (number)
    {
      case 0: result = "right " + part_name; break;
      case 1: result = "center " + part_name; break;
      case 2: result = "left " + part_name; break;
      default: break;
    }
    break;

  case 4: // Legs/feet assume front/rear, others assume upper/lower.
    if ((part == BodyPart::Leg) || (part == BodyPart::Foot))
    {
      switch (number)
      {
        case 0: result = "front right " + part_name; break;
        case 1: result = "front left " + part_name; break;
        case 2: result = "rear right " + part_name; break;
        case 3: result = "rear left " + part_name; break;
        default: break;
      }
    }
    else
    {
      switch (number)
      {
        case 0: result = "upper right " + part_name; break;
        case 1: result = "upper left " + part_name; break;
        case 2: result = "lower right " + part_name; break;
        case 3: result = "lower left " + part_name; break;
        default: break;
      }
    }
    break;

  case 6: // Legs/feet assume front/middle/rear, others upper/middle/lower.
    if ((part == BodyPart::Leg) || (part == BodyPart::Foot))
    {
      switch (number)
      {
        case 0: result = "front right " + part_name; break;
        case 1: result = "front left " + part_name; break;
        case 2: result = "middle right " + part_name; break;
        case 3: result = "middle left " + part_name; break;
        case 4: result = "rear right " + part_name; break;
        case 5: result = "rear left " + part_name; break;
        default: break;
      }
    }
    else
    {
      switch (number)
      {
        case 0: result = "upper right " + part_name; break;
        case 1: result = "upper left " + part_name; break;
        case 2: result = "middle right " + part_name; break;
        case 3: result = "middle left " + part_name; break;
        case 4: result = "lower right " + part_name; break;
        case 5: result = "lower left " + part_name; break;
        default: break;
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


std::string const& Entity::get_subject_pronoun() const
{
  return getSubjPro(get_gender());
}

std::string const& Entity::get_object_pronoun() const
{
  return getObjPro(get_gender());
}

std::string const& Entity::get_reflexive_pronoun() const
{
  return getRefPro(get_gender());
}

std::string const& Entity::get_possessive_adjective() const
{
  return getPossAdj(get_gender());
}

std::string const& Entity::get_possessive_pronoun() const
{
  return getPossPro(get_gender());
}

AttributeSet& Entity::get_attributes()
{
  return impl->attributes;
}

AttributeSet const& Entity::get_attributes() const
{
  return impl->attributes;
}

bool Entity::is_player() const
{
  return (TF.get_player().get() == this);
}

void Entity::queue_action(Action action)
{
  impl->actions.push_back(action);
}

bool Entity::pending_action() const
{
  return !(impl->actions.empty());
}

bool Entity::set_ai_strategy(AIStrategy* strategy_ptr)
{
  if (strategy_ptr != nullptr)
  {
    impl->ai_strategy.reset(strategy_ptr);
    return true;
  }
  else
  {
    return false;
  }
}

bool Entity::is_wielding(Thing& thing)
{
  unsigned int dummy;
  return is_wielding(thing, dummy);
}

bool Entity::is_wielding(Thing& thing, unsigned int& hand)
{
  return impl->is_wielding(&thing, hand);
}

bool Entity::has_equipped(Thing& thing)
{
  WearLocation dummy;
  return has_equipped(thing, dummy);
}

bool Entity::has_equipped(Thing& thing, WearLocation& location)
{
  return impl->is_wearing(&thing, location);
}

bool Entity::can_reach(Thing& thing)
{
  // Check if it is our location.
  auto our_location = get_location();
  if (our_location.get() == &thing)
  {
    return true;
  }

  // Check if it's at our location.
  auto thing_location = thing.get_location();
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

bool Entity::attack(Thing& thing, unsigned int& action_time)
{
  std::string message;

  bool reachable = this->can_reach(thing);
  /// @todo deal with Entities in your Inventory -- WTF do you do THEN?

  if (reachable)
  {
    /// @todo Write attack code.
    the_message_log.add("TODO: This is where you would attack something.");
  }

  return false;
}

ActionResult Entity::can_drink(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (&thing == this)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the thing is within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  // Check that it is something that contains a liquid.
  if (!thing.is_liquid_carrier())
  {
    return ActionResult::FailureNotLiquidCarrier;
  }

  return ActionResult::Success;
}

bool Entity::drink(Thing& thing, unsigned int& action_time)
{
  std::string message;

  ActionResult drink_try = this->can_drink(thing, action_time);

  switch (drink_try)
  {
  case ActionResult::Success:
    if (thing.drinkable_by(*this))
    {
      message = _YOU_ + " drink " + thing.get_name();
      the_message_log.add(message);

      if (thing.perform_action_drank_by(*this))
      {
        return true;
      }
      else
      {
        message = _YOU_ + " stop drinking.";
        the_message_log.add(message);
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    if (this == TF.get_player().get())
    {
      message = _YOU_TRY_ + " to drink " + thing.get_name() + ".";
      the_message_log.add(message);

      /// @todo When drinking self, special message if we're a liquid-based organism.
      message = "That is a particularly unsettling image.";
      the_message_log.add(message);
    }
    else
    {
      message = _YOU_TRY_ + " to drink " + _YOURSELF_ +
                ", which seriously shouldn't happen.";
      the_message_log.add(message);
      MINOR_ERROR("Non-player Entity tried to drink self!?");
    }
    break;

  case ActionResult::FailureNotLiquidCarrier:
    message = _YOU_TRY_ + " to drink " + thing.get_name() + ".";
    the_message_log.add(message);
    message = _YOU_ + " cannot drink from that!";
    the_message_log.add(message);
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", drink_try);
    break;
  }

  return false;
}

ActionResult Entity::can_drop(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (&thing == this)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's in our inventory.
  if (!this->get_inventory().contains(thing))
  {
    return ActionResult::FailureNotPresent;
  }

  // Check that we're not wielding the item.
  if (this->is_wielding(thing))
  {
    return ActionResult::FailureItemWielded;
  }

  /// @todo Check that we're not wearing the item.

  return ActionResult::Success;
}

bool Entity::drop(Thing& thing, unsigned int& action_time)
{
  std::string message;
  std::shared_ptr<Thing> entity_location = get_location();

  ActionResult drop_try = this->can_drop(thing, action_time);

  switch (drop_try)
  {
  case ActionResult::Success:
    {
      if (thing.is_movable())
      {
        if (entity_location->can_contain(thing) == ActionResult::Success)
        {
          if (thing.perform_action_dropped_by(*this))
          {
            message = _YOU_ + choose_verb(" drop ", " drops ") +
                      thing.get_name() + ".";
            the_message_log.add(message);
            if (thing.move_into(entity_location))
            {
              return true;
            }
            else
            {
              MAJOR_ERROR("Could not move Thing even though "
                          "is_movable returned Success");
              break;
            }
          }
          else // Drop failed
          {
            // perform_action_dropped_by() will print any relevant messages
          }
        }
        else // can't contain the thing
        {
          // This is mighty strange, but I suppose there might be MapTiles in
          // the future that can't contain certain Things.
          message = _YOU_TRY_ + " to drop " + thing.get_name() + ".";
          the_message_log.add(message);

          message = entity_location->get_name() + " cannot hold " +
                    thing.get_name() + ".";
          the_message_log.add(message);
        }
      }
      else // can't be moved
      {
        message = _YOU_TRY_ + " to drop " + thing.get_name() + ".";
        the_message_log.add(message);

        message = thing.get_name() + " cannot be moved.";
        the_message_log.add(message);
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (this == TF.get_player().get())
      {
        message = "Drop yourself?  What, you mean commit suicide?  Uh, no.";
      }
      else
      {
        message = _YOU_TRY_ + " to drop " + _YOURSELF_ +
                  ", which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to drop self!?");
      }
      the_message_log.add(message);
      break;
    }

  case ActionResult::FailureNotPresent:
    {
      message = _YOU_TRY_ + " to drop " + thing.get_name() + ".";
      the_message_log.add(message);

      message = thing.get_name() + thing.choose_verb(" are", " is") +
                " not in " + _YOUR_ + " inventory!";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureItemEquipped:
    {
      message = _YOU_TRY_ + " to drop " + thing.get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ + " cannot drop something that is currently being worn.";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureItemWielded:
    {
      message = _YOU_TRY_ + " to drop " + thing.get_name() + ".";
      the_message_log.add(message);

      /// @todo Perhaps automatically try to unwield the item before dropping?
      message = _YOU_ + " cannot drop something that is currently being wielded.";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", drop_try);
    break;
  }

  return false;
}

ActionResult Entity::can_eat(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (&thing == this)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the thing is within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  return ActionResult::Success;
}

bool Entity::eat(Thing& thing, unsigned int& action_time)
{
  std::string message;

  ActionResult eat_try = this->can_eat(thing, action_time);

  message = _YOU_TRY_ + " to eat " + thing.get_name() + ".";
  the_message_log.add(message);

  switch (eat_try)
  {
  case ActionResult::Success:
    if (thing.edible_by(*this))
    {
      if (thing.perform_action_eaten_by(*this))
      {
        return true;
      }
    }
    else
    {
      message = _YOU_ + " can't eat that!";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureSelfReference:
    if (this == TF.get_player().get())
    {
      /// @todo When eating self, special message if we're a liquid-based organism.
      message = "But you really aren't that tasty, so you stop.";
      the_message_log.add(message);
    }
    else
    {
      message = "That seriously shouldn't happen!";
      the_message_log.add(message);

      MINOR_ERROR("Non-player Entity tried to eat self!?");
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", eat_try);
    break;
  }
  return false;
}

ActionResult Entity::can_mix(Thing& thing1, Thing& thing2,
                             unsigned int& action_time)
{
  action_time = 1;

  // Check that they aren't both the same thing.
  if (&thing1 == &thing2)
  {
    return ActionResult::FailureCircularReference;
  }

  // Check that neither of them is us.
  if (&thing1 == this || &thing2 == this)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that both are within reach.
  if (!this->can_reach(thing1) || !this->can_reach(thing2))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  /// @todo write Entity::can_mix
  return ActionResult::Success;
}

bool Entity::mix(Thing& thing1, Thing& thing2, unsigned int& action_time)
{
  /// @todo write Entity::mix
  return false;
}

bool Entity::move(Direction new_direction,
                  unsigned int& action_time)
{
  /// @todo Update action time based on direction, speed, etc.

  std::string message;

  // First: check direction.
  if (new_direction == Direction::Self)
  {
    message = _YOU_ + " successfully" + choose_verb(" stay", " stays") +
              " where " + get_subject_pronoun() + _ARE_ + ".";
    the_message_log.add(message);
    return true;
  }

  std::shared_ptr<Thing> location = get_location();
  MapTile* current_tile = get_maptile();

  // Make sure we're not in limbo!
  if (!location || (current_tile == nullptr))
  {
    message = _YOU_ + " can't move because " + _YOU_DO_ + " not exist physically!";
    the_message_log.add(message);
    return false;
  }

  // Make sure we CAN move!
  if (!can_currently_move())
  {
    message = _YOU_ + choose_verb(" do", " does") +
              " not have the capability of movement.";
    the_message_log.add(message);
    return false;
  }

  // Make sure we're not confined inside another thing.
  if (is_inside_another_thing())
  {
    message = _YOU_ARE_ + " inside " + location->get_indef_name() +
              " and " + _ARE_ + " not going anywhere!";

    the_message_log.add(message);
    return false;
  }

  if (new_direction == Direction::Up)
  {
    /// @todo Write up/down movement code
    message = "Up/down movement is not yet supported!";
    the_message_log.add(message);
    return false;
  }
  else if (new_direction == Direction::Down)
  {
    /// @todo Write up/down movement code
    message = "Up/down movement is not yet supported!";
    the_message_log.add(message);
    return false;
  }
  else
  {
    // Figure out our target location.
    sf::Vector2i coords = current_tile->get_coords();
    int x_offset = get_x_offset(new_direction);
    int y_offset = get_y_offset(new_direction);
    int x_new = coords.x + x_offset;
    int y_new = coords.y + y_offset;
    Map& current_map = MF.get(get_map_id());
    sf::Vector2i map_size = current_map.get_size();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
        (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      message = _YOU_ + " can't move there; it is out of bounds!";
      the_message_log.add(message);
      return false;
    }

    auto& new_tile = current_map.get_tile(x_new, y_new);
    auto new_floor = new_tile->get_floor();

    if (new_tile->can_be_traversed_by(*this))
    {
      return move_into(new_floor);
    }
    else
    {
      std::string tile_description = getMapTileTypeDescription(new_tile->get_type());
      message = _YOU_ARE_ + " stopped by " +
                getIndefArt(tile_description) + " " +
                tile_description + ".";
      the_message_log.add(message);
      return false;
    }
  } // end else if (other direction)
}

ActionResult Entity::can_pick_up(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (&thing == this)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check if it's already in our inventory.
  if (this->get_inventory().contains(thing))
  {
    return ActionResult::FailureAlreadyPresent;
  }

  // Check that it's within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  /// @todo When picking up, check if our inventory is full-up.
  return ActionResult::Success;
}

bool Entity::pick_up(Thing& thing, unsigned int& action_time)
{
  std::string message;

  ActionResult pick_up_try = this->can_pick_up(thing, action_time);

  switch (pick_up_try)
  {
    case ActionResult::Success:
      if (thing.is_movable())
      {
        if (thing.perform_action_picked_up_by(*this))
        {
          message = _YOU_ + choose_verb(" pick", " picks") + " up " +
                    thing.get_name() + ".";
          the_message_log.add(message);
          if (thing.move_into(shared_from_this()))
          {
            return true;
          }
          else // could not add to inventory
          {
            MAJOR_ERROR("Could not move Thing even though "
                        "is_movable returned Success");
            break;
          }
        }
        else // perform_action_picked_up_by(*this) returned false
        {
          // perform_action_picked_up_by() will print any relevant messages
        }
      }
      else // thing cannot be moved
      {
        message = _YOU_TRY_ +
                  " to pick up " + thing.get_name();
        the_message_log.add(message);

        message = thing.get_name() + " cannot be moved.";
        the_message_log.add(message);
      }
      break;
    case ActionResult::FailureSelfReference:
      {
        if (is_player())
        {
          message = "Oh, ha ha, I get it, \"pick me up\".  Nice try.";
        }
        else
        {
          message = _YOU_TRY_ +
                    " to pick " + _YOURSELF_ +
                    "up, which seriously shouldn't happen.";
          MINOR_ERROR("Non-player Entity tried to pick self up!?");
        }
        the_message_log.add(message);
        break;
      }
    case ActionResult::FailureInventoryFull:
      {
        message = _YOU_TRY_ +
                  " to pick up " + thing.get_name();
        the_message_log.add(message);

        message = _YOUR_ + " inventory cannot accomodate " + thing.get_name();
        the_message_log.add(message);
      }
      break;
    case ActionResult::FailureAlreadyPresent:
      {
        message = _YOU_TRY_ +
                  " to pick up " + thing.get_name();
        the_message_log.add(message);

        message = thing.get_name() + " is already in " +
                  _YOUR_ + " inventory!";
        the_message_log.add(message);
      }
      break;
    case ActionResult::FailureThingOutOfReach:
      {
        message = _YOU_TRY_ +
                  " to pick up " + thing.get_name();
        the_message_log.add(message);

        message = thing.get_name() + " is out of " + _YOUR_ + " reach.";
        the_message_log.add(message);
      }
      break;
    default:
      MINOR_ERROR("Unknown ActionResult %d", pick_up_try);
      break;
  }

  action_time = 0;
  return false;
}

ActionResult Entity::can_put_into(Thing& thing, Thing& container,
                                  unsigned int& action_time)
{
  action_time = 1;

  // Check that the thing and container aren't the same thing.
  if (&thing == &container)
  {
    return ActionResult::FailureCircularReference;
  }

  // Check that the thing is not US!
  if ((&thing == this) || (&container == this))
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the container is not US!
  if ((&thing == this) || (&container == this))
  {
    return ActionResult::FailureContainerCantBeSelf;
  }

  // Check that the container actually IS a container.
  if (container.get_inventory_size() == 0)
  {
    return ActionResult::FailureTargetNotAContainer;
  }

  // Check that the thing's location isn't already the container.
  if (thing.get_location().get() == &container)
  {
    return ActionResult::FailureAlreadyPresent;
  }

  // Check that the thing is within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  // Check that the container is within reach.
  if (!this->can_reach(container))
  {
    return ActionResult::FailureContainerOutOfReach;
  }

  // Make sure the container can hold this thing.
  return container.can_contain(*this);
}

bool Entity::put_into(Thing& thing, std::shared_ptr<Thing> container_sptr,
                      unsigned int& action_time)
{
  std::string message;

  ASSERT_CONDITION(container_sptr);
  auto& container = *(container_sptr.get());

  ActionResult put_try = this->can_put_into(thing, container, action_time);

  switch (put_try)
  {
  case ActionResult::Success:
    {
      if (thing.perform_action_put_into(container))
      {
        message = _YOU_ + choose_verb(" place ", "places ") +
                  thing.get_name() + " into " +
                  container.get_name() + ".";
        the_message_log.add(message);
        if (!thing.move_into(container_sptr))
        {
          MAJOR_ERROR("Could not move Thing into Container");
        }
        else
        {
          return true;
        }
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (is_player())
      {
        /// @todo Possibly allow player to voluntarily enter a container?
        message = "I'm afraid you can't do that.  "
                  "(At least, not in this version...)";
      }
      else
      {
        message = _YOU_TRY_ + " to store " + _YOURSELF_ +
                  "into the " + thing.get_name() +
                  ", which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to store self!?");
      }
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureContainerCantBeSelf:
    {
      if (is_player())
      {
        message = "Store something in yourself?  "
                  "What do you think you are, a drug mule?";
      }
      else
      {
        message = _YOU_TRY_ + " to store " + thing.get_name() +
                  "into " + _YOURSELF_ +
                  ", which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to store into self!?");
      }
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureAlreadyPresent:
    {
      message = _YOU_TRY_ + " to store " + thing.get_name() + " in " +
                container.get_name() + ".";
      the_message_log.add(message);

      message = thing.get_name() + " is already in " +
                container.get_name() + "!";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureTargetNotAContainer:
    {
      message = _YOU_TRY_ + " to store " + thing.get_name() + " in " +
                container.get_name() + ".";
      the_message_log.add(message);

      message = container.get_name() + " is not a container!";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureThingOutOfReach:
    {
      message = _YOU_TRY_ + " to store " + thing.get_name() + " in " +
                container.get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ + " cannot reach " + thing.get_name() + ".";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureContainerOutOfReach:
    {
      message = _YOU_TRY_ + " to store " + thing.get_name() + " in " +
                container.get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ + " cannot reach " + container.get_name() + ".";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureCircularReference:
    {
      if (is_player())
      {
        message = "That would be an interesting topological exercise.";
      }
      else
      {
        message = _YOU_TRY_ + " to store " + thing.get_name() +
                  "in itself, which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to store a container in itself!?");
      }
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", put_try);
    break;
  }

  return false;
}

ActionResult Entity::can_read(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that the thing is within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  if (0) ///< @todo Intelligence tests for reading.
  {
    return ActionResult::FailureTooStupid;
  }

  return ActionResult::Success;
}

bool Entity::read(Thing& thing, unsigned int& action_time)
{
  std::string message;

  ActionResult read_try = this->can_read(thing, action_time);

  switch (read_try)
  {
  case ActionResult::Success:
    {
      if (thing.readable_by(*this))
      {
        switch (thing.perform_action_read_by(*this))
        {
        case ActionResult::SuccessDestroyed:
          thing.destroy();
          return true;

        case ActionResult::Success:
          return true;

        case ActionResult::Failure:
        default:
          return false;
        }
      }
      else
      {
        message = _YOU_TRY_ + " to read " + thing.get_name() + ".";
        the_message_log.add(message);

        message = thing.get_name() + " has no writing on it to read.";
        the_message_log.add(message);
      }
    }
    break;

  case ActionResult::FailureTooStupid:
    {
      message = _YOU_TRY_ + " to read " + thing.get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ARE_ + " not smart enough to read " +
                thing.get_name() + ".";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", read_try);
    break;
  }

  return false;
}

ActionResult Entity::can_take_out(Thing& thing,
                                  unsigned int& action_time)
{
  action_time = 1;

  // Check that the thing isn't US!
  if (thing.is_player())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the container is not a MapTile or Entity.
  if (!thing.is_inside_another_thing())
  {
    return ActionResult::FailureNotInsideContainer;
  }

  std::shared_ptr<Thing> container = thing.get_location();
  ASSERT_CONDITION(container);

  // Check that the container is within reach.
  if (!can_reach(*(container.get())))
  {
    return ActionResult::FailureContainerOutOfReach;
  }

  return ActionResult::Success;
}


bool Entity::take_out(Thing &thing,
                      unsigned int& action_time)
{
  std::string message;

  ActionResult takeout_try = this->can_take_out(thing, action_time);

  std::shared_ptr<Thing> container = thing.get_location();
  ASSERT_CONDITION(container);

  auto new_location = container->get_location();

  switch (takeout_try)
  {
  case ActionResult::Success:
    {
      if (thing.perform_action_take_out())
      {
        if (!thing.move_into(new_location))
        {
          MAJOR_ERROR("Could not move Thing out of Container");
        }
        else
        {
          message = _YOU_ + choose_verb(" remove ", "removes ") +
                    thing.get_name() + " from " +
                    container->get_name() + ".";
          the_message_log.add(message);
        }
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (is_player())
      {
        /// @todo Maybe allow player to voluntarily exit a container?
        message = "I'm afraid you can't do that.  "
                  "(At least, not in this version...)";
      }
      else
      {
        message = _YOU_TRY_ + " to take " + _YOURSELF_ +
                  "out, which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to take self out!?");
      }
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureNotInsideContainer:
    {
      message = _YOU_TRY_ + " to remove " + thing.get_name() +
                " from its container.";
      the_message_log.add(message);

      message = "But " + thing.get_name() + " is not inside a container!";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureContainerOutOfReach:
    {
      message = _YOU_TRY_ + " to remove " + thing.get_name() + " from " +
                container->get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ + " cannot reach " + container->get_name() + ".";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", takeout_try);
    break;
  }

  return false;
}

ActionResult Entity::can_toss(Thing& thing,
                              unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing.is_player())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's in our inventory.
  if (!this->get_inventory().contains(thing))
  {
    return ActionResult::FailureNotPresent;
  }

  /// @todo Check that we're not wearing the item.

  return ActionResult::Success;
}

bool Entity::toss(Thing& thing, Direction& direction,
                  unsigned int& action_time)
{
  std::string message;
  ActionResult toss_try = this->can_toss(thing, action_time);

  switch (toss_try)
  {
  case ActionResult::Success:
    {
      auto new_location = get_location();
      if (thing.is_movable())
      {
        if (thing.perform_action_thrown_by(*this, direction))
        {
          if (thing.move_into(new_location))
          {
            message = _YOU_ + choose_verb(" throw ", " throws ") +
                      thing.get_name();
            the_message_log.add(message);

            /// @todo When throwing, set Thing's direction and velocity
            return true;
          }
          else
          {
            MAJOR_ERROR("Could not move Thing even though "
                        "is_movable returned Success");
          }
        }
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (is_player())
      {
        message = "Throw yourself?  Throw yourself what, a party?";
      }
      else
      {
        message =  _YOU_TRY_ + " to throw " + _YOURSELF_ +
                  ", which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to throw self!?");
      }
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureItemEquipped:
    {
      message = _YOU_ + " cannot throw something " + _YOU_ARE_ + "wearing.";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureNotPresent:
    {
      message = _YOU_TRY_ + " to throw " + thing.get_name() + ".";
      the_message_log.add(message);

      message = "But " + thing.get_name() +
                thing.choose_verb(" are", " is") +
                " not actually in " + _YOUR_ +
                " inventory!";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", toss_try);
    break;
  }

  return false;
}

ActionResult Entity::can_deequip(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing.is_player())
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

bool Entity::deequip(Thing& thing, unsigned int& action_time)
{
  std::string message;
  ActionResult deequip_try = this->can_deequip(thing, action_time);
  std::string thing_name = thing.get_name();

  message = _YOU_TRY_ + " to take off " + thing_name;
  the_message_log.add(message);

  switch (deequip_try)
  {
    case ActionResult::Success:
      {
        // Get the body part this item is equipped on.
        WearLocation location;
        this->has_equipped(thing, location);

        if (thing.perform_action_deequipped_by(*this, location))
        {
          impl->deequip(thing);

          std::string wear_desc = get_bodypart_description(location.part,
                                                           location.number);
          message = _YOU_ARE_ + " no longer wearing " + thing_name +
                    " on " + _YOUR_ + " " + wear_desc + ".";
          the_message_log.add(message);
          return true;
        }
      }
      break;

    case ActionResult::FailureItemNotEquipped:
      {
        message = _YOU_ARE_ + " not wearing " + thing_name + ".";
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

ActionResult Entity::can_equip(Thing& thing, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (&thing == this)
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  std::string message;
  BodyPart part = thing.equippable_on();

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

bool Entity::equip(std::shared_ptr<Thing> thing, unsigned int& action_time)
{
  std::string message;

  ASSERT_CONDITION(thing);

  ActionResult equip_try = this->can_equip(*(thing.get()), action_time);
  std::string thing_name = thing->get_name();

  switch (equip_try)
  {
    case ActionResult::Success:
      {
        WearLocation location;

        if (thing->perform_action_equipped_by(*this, location))
        {
          impl->equip(thing, location);
          std::string wear_desc = get_bodypart_description(location.part,
                                                           location.number);
          message = _YOU_ARE_ + " now wearing " + thing_name +
                    " on " + _YOUR_ + " " + wear_desc + ".";
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
        message = _YOU_TRY_ + " to equip " + _YOURSELF_ +
                  ", which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to equip self!?");
      }
      the_message_log.add(message);
      break;

    case ActionResult::FailureThingOutOfReach:
      {
        message = _YOU_TRY_ + " to equip " + thing_name + ".";
        the_message_log.add(message);

        message = _YOU_ + " cannot reach " + thing_name + ".";
        the_message_log.add(message);
      }
      break;

    case ActionResult::FailureItemNotEquippable:
      {
        message = _YOU_TRY_ + " to equip " + thing_name + ".";
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

ActionResult Entity::can_wield(Thing& thing,
                               unsigned int hand,
                               unsigned int& action_time)
{
  action_time = 1;

  // Check that it's within reach.
  if (!this->can_reach(thing))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  /// @todo Check that we have hands capable of wielding anything.

  return ActionResult::Success;
}

bool Entity::wield(std::shared_ptr<Thing> thing,
                   unsigned int hand,
                   unsigned int& action_time)
{
  std::string message;
  std::string bodypart_desc =
    this->get_bodypart_description(BodyPart::Hand, hand);

  auto currently_wielded = impl->wielding_in(hand);

  std::string thing_name = thing ? thing->get_name() : "nothing";

  // First, check if we're already wielding something.
  if (currently_wielded)
  {
    // Now, check if the thing we're already wielding is THIS thing.
    if (currently_wielded.get() == thing.get())
    {
      message = _YOU_ARE_ + " already wielding " + thing_name + " with " +
                _YOUR_ + " " + bodypart_desc + ".";
      the_message_log.add(message);
      return true;
    }
    else
    {
      // Try to unwield the old item.
      if (currently_wielded->perform_action_unwielded_by(*this))
      {
        impl->unwield(*(currently_wielded.get()));
      }
      else
      {
        return false;
      }
    }
  }

  // If we HAVE a new item, try to wield it.
  ActionResult wield_try = thing ? this->can_wield(*(thing.get()), hand, action_time)
                                 : ActionResult::SuccessSelfReference;

  switch (wield_try)
  {
  case ActionResult::Success:
  case ActionResult::SuccessSwapHands:
    {
      if (thing->perform_action_wielded_by(*this))
      {
        impl->wield(thing, hand);
        message = _YOU_ARE_ + " now wielding " + thing_name +
                  " with " + _YOUR_ + " " + bodypart_desc + ".";
        the_message_log.add(message);
        return true;
      }
    }
    break;

  case ActionResult::SuccessSelfReference:
    {
      message = _YOU_ARE_ + " no longer wielding any weapons with " +
                _YOUR_ + " " +
                this->get_bodypart_description(BodyPart::Hand, hand) + ".";
      the_message_log.add(message);
      return true;
    }
    break;

  case ActionResult::FailureThingOutOfReach:
    {
      message = _YOU_TRY_ + " to wield " + thing_name + ".";
      the_message_log.add(message);

      message = _YOU_ + " cannot reach " + thing_name + ".";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureNotEnoughHands:
    {
      message = _YOU_TRY_ + " to wield " + thing_name;
      the_message_log.add(message);

      message = _YOU_ + choose_verb(" don't", " doesn't") +
                " have enough free " +
                this->get_bodypart_plural(BodyPart::Hand) + ".";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", wield_try);
    break;
  }
  return false;
}

bool Entity::can_currently_see() const
{
  return true;
}

bool Entity::can_currently_move() const
{
  return true;
}

void Entity::light_up_surroundings()
{
  if (get_inventory_size() != 0)
  {
    auto& things = get_inventory().get_things();
    for (auto& thing : things)
    {
      thing.second->light_up_surroundings();
    }
  }

  _light_up_surroundings();
}

void Entity::be_lit_by(LightSource& light)
{
  _be_lit_by(light);

  auto location = get_location();
  if (location)
  {
    location->be_lit_by(light);
  }
}

// *** PROTECTED METHODS ******************************************************

bool Entity::_do_process()
{
  unsigned int action_time = 0;
  bool success = false;

  // If entity is currently busy, decrement by one and return.
  if (impl->busy_counter > 0)
  {
    --(impl->busy_counter);
    return true;
  }

  // Perform any subclass-specific processing.
  // Useful if, for example, your Entity can rise from the dead.
  _do_process_specific();

  // Is the entity dead?
  if (impl->attributes.get(Attribute::HP) > 0)
  {
    // If the entity is not the player, perform the AI strategy associated with
    // it.
    if (!is_player())
    {
      if (impl->ai_strategy.get() != nullptr)
      impl->ai_strategy->execute();
    }

    // If actions are pending...
    if (!impl->actions.empty())
    {
      Action action = impl->actions.front();
      impl->actions.pop_front();

      unsigned int number_of_things = action.things.size();

      switch (action.type)
      {
      case Action::Type::Wait:
        success = this->move(Direction::Self, action_time);
        if (success)
        {
          impl->busy_counter += action_time;
        }
        break;

      case Action::Type::Move:
        success = this->move(action.direction, action_time);
        if (success)
        {
          impl->busy_counter += action_time;
        }
        break;

      case Action::Type::Drop:
        for (auto thing_wptr : action.things)
        {
          auto thing = thing_wptr.lock();
          if (thing != nullptr)
          {
            success = this->drop(*thing, action_time);
            if (success)
            {
              impl->busy_counter += action_time;
            }
          }
        }
        break;

      case Action::Type::Eat:
        for (auto thing_wptr : action.things)
        {
          auto thing = thing_wptr.lock();
          if (thing != nullptr)
          {
            success = this->eat(*thing, action_time);
            if (success)
            {
              impl->busy_counter += action_time;
            }
          }
        }
        break;

      case Action::Type::Pickup:
        for (auto thing_wptr : action.things)
        {
          auto thing = thing_wptr.lock();
          if (thing != nullptr)
          {
            success = this->pick_up(*thing, action_time);
            if (success)
            {
              impl->busy_counter += action_time;
            }
          }
        }
        break;

      case Action::Type::Quaff:
        for (auto thing_wptr : action.things)
        {
          auto thing = thing_wptr.lock();
          if (thing != nullptr)
          {
            success = this->drink(*thing, action_time);
            if (success)
            {
              impl->busy_counter += action_time;
            }
          }
        }
        break;

      case Action::Type::Store:
      {
        auto container = action.target.lock();
        if (container && container->get_inventory_size() != 0)
        {
          for (auto thing_wptr : action.things)
          {
            auto thing = thing_wptr.lock();
            if (thing != nullptr)
            {
              success = this->put_into(*thing, container, action_time);
              if (success)
              {
                impl->busy_counter += action_time;
              }
            }
          }
        }
        else
        {
          the_message_log.add("That target is not a container.");
        }
        break;
      }

      case Action::Type::TakeOut:
        for (auto thing_wptr : action.things)
        {
          auto thing = thing_wptr.lock();
          if (thing != nullptr)
          {
            success = this->take_out(*thing, action_time);
            if (success)
            {
              impl->busy_counter += action_time;
            }
          }
        }
        break;

      case Action::Type::Wield:
      {
        if (number_of_things > 1)
        {
          the_message_log.add("NOTE: Only wielding the last item selected.");
        }

        auto thing = action.things[number_of_things - 1].lock();
        if (thing != nullptr)
        {
          /// @todo Implement wielding using other hands.
          success = this->wield(thing, 0, action_time);
          if (success)
          {
            impl->busy_counter += action_time;
          }
        }
        break;
      }

      default:
        MINOR_ERROR("Unimplemented action.type %d", action.type);
        break;
      } // end switch (action)
    } // end if (actions pending)
  } // end if (HP > 0)

  return true;
}

void Entity::_do_process_specific()
{
  // Default implementation does nothing.
}

bool Entity::dec_busy_counter()
{
  if (impl->busy_counter != 0)
  {
    --impl->busy_counter;
  }

  return (impl->busy_counter == 0);
}

void Entity::set_busy_counter(int value)
{
  impl->busy_counter = value;
}

std::vector<MapTileType>& Entity::get_map_memory()
{
  return impl->map_memory;
}

void Entity::do_recursive_visibility(int octant,
                                     int depth,
                                     float slope_A,
                                     float slope_B)
{
  int x = 0;
  int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (is_inside_another_thing())
  {
    return;
  }

  MapTile* tile = get_maptile();
  sf::Vector2i tile_coords = tile->get_coords();
  Map& game_map = MF.get(get_map_id());
  int eX = tile_coords.x;
  int eY = tile_coords.y;

  static const int mv = 128;
  static constexpr int mw = (mv * mv);

  switch (octant)
  {
    case 1:
      y = eY - depth;
      x = rintf(static_cast<float>(eX) - (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x - 1, y)->is_opaque())
            {
              do_recursive_visibility(1, depth + 1, slope_A,
                                      calc_slope(x - 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x - 1, y)->is_opaque())
            {
              slope_A = calc_slope(x - 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        ++x;
      }
      --x;
      break;
    case 2:
      y = eY - depth;
      x = rintf(static_cast<float>(eX) + (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x + 1, y)->is_opaque())
            {
              do_recursive_visibility(2, depth + 1, slope_A,
                                      calc_slope(x + 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x + 1, y)->is_opaque())
            {
              slope_A = -calc_slope(x + 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        --x;
      }
      ++x;
      break;
    case 3:
      x = eX + depth;
      y = rintf(static_cast<float>(eY) - (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x, y - 1)->is_opaque())
            {
              do_recursive_visibility(3, depth + 1, slope_A,
                                      calc_inv_slope(x - 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y - 1)->is_opaque())
            {
              slope_A = -calc_inv_slope(x + 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        ++y;
      }
      --y;
      break;
    case 4:
      x = eX + depth;
      y = rintf(static_cast<float>(eY) + (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x, y + 1)->is_opaque())
            {
              do_recursive_visibility(4, depth + 1, slope_A,
                                      calc_inv_slope(x - 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y + 1)->is_opaque())
            {
              slope_A = calc_inv_slope(x + 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        --y;
      }
      ++y;
      break;
    case 5:
      y = eY + depth;
      x = rintf(static_cast<float>(eX) + (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x + 1, y)->is_opaque())
            {
              do_recursive_visibility(5, depth + 1, slope_A,
                                      calc_slope(x + 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x + 1, y)->is_opaque())
            {
              slope_A = calc_slope(x + 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        --x;
      }
      ++x;
      break;
    case 6:
      y = eY + depth;
      x = rintf(static_cast<float>(eX) - (slope_A * static_cast<float>(depth)));
      while (calc_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x - 1, y)->is_opaque())
            {
              do_recursive_visibility(6, depth + 1, slope_A,
                                      calc_slope(x - 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x - 1, y)->is_opaque())
            {
              slope_A = -calc_slope(x - 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        ++x;
      }
      --x;
      break;
    case 7:
      x = eX - depth;
      y = rintf(static_cast<float>(eY) + (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) <= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x, y + 1)->is_opaque())
            {
              do_recursive_visibility(7, depth + 1, slope_A,
                                      calc_inv_slope(x + 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y + 1)->is_opaque())
            {
              slope_A = -calc_inv_slope(x - 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        --y;
      }
      ++y;
      break;
    case 8:
      x = eX - depth;
      y = rintf(static_cast<float>(eY) - (slope_A * static_cast<float>(depth)));
      while (calc_inv_slope(x, y, eX, eY) >= slope_B)
      {
        if (calc_vis_distance(x, y, eX, eY) <= mw)
        {
          if (game_map.get_tile(x, y)->is_opaque())
          {
            if (!game_map.get_tile(x, y - 1)->is_opaque())
            {
              do_recursive_visibility(8, depth + 1, slope_A,
                                      calc_inv_slope(x + 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y - 1)->is_opaque())
            {
              slope_A = calc_inv_slope(x - 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y)->get_type();
        }
        ++y;
      }
      --y;
      break;
    default:
      MAJOR_ERROR("Octant passed to do_recursive_visibility was %d (not 1 to 8)!", octant);
      break;
  }

  if ((depth < mv) && (!game_map.get_tile(x, y)->is_opaque()))
  {
    do_recursive_visibility(octant, depth + 1, slope_A, slope_B);
  }
}
