#include "Entity.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <sstream>

#include "App.h"
#include "ConfigSettings.h"
#include "Container.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
#include "IsType.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "MessageLog.h"
#include "Ordinal.h"
#include "ThingFactory.h"

#include "ai/AIStrategy.h"

// Local definitions to make reading/writing status info a bit easier.
#define _YOU_   (this->get_name())
#define _ARE_   (this->choose_verb(" are", " is"))
#define _YOUR_  (this->get_possessive())
#define _HAVE_  (this->choose_verb(" have", " has"))
#define _YOURSELF_ (this->get_reflexive_pronoun())
#define _TRY_   (this->choose_verb(" try", " tries"))

#define _YOU_ARE_ _YOU_ + _ARE_
#define _YOU_TRY_ _YOU_ + _TRY_

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

  /// Map of items wielded, by ThingId.
  std::map<ThingId, unsigned int> wielded_items_by_thing_id;

  /// Map of items wielded, by hand number.
  std::map<unsigned int, ThingId> wielded_items_by_hand;

  /// Map of things worn, by ThingId.
  std::map<ThingId, WearLocation> equipped_items_by_thing_id;

  bool is_wielding_thing(ThingId thing_id, unsigned int& hand)
  {
    if (wielded_items_by_thing_id.count(thing_id) == 0)
    {
      return false;
    }
    else
    {
      hand = wielded_items_by_thing_id[thing_id];
      return true;
    }
  }

  bool is_wielding_in(unsigned int hand, ThingId& thing_id)
  {
    if (wielded_items_by_hand.count(hand) == 0)
    {
      return false;
    }
    else
    {
      thing_id = wielded_items_by_hand[hand];
      return true;
    }
  }

  bool is_wearing_thing(ThingId thing_id, WearLocation& location)
  {
    if (equipped_items_by_thing_id.count(thing_id) == 0)
    {
      return false;
    }
    else
    {
      location = equipped_items_by_thing_id[thing_id];
      return true;
    }
  }

  void wield(ThingId thing_id, unsigned int hand)
  {
    wielded_items_by_hand[hand] = thing_id;
    wielded_items_by_thing_id[thing_id] = hand;
  }

  bool unwield(ThingId thing_id)
  {
    if (wielded_items_by_thing_id.count(thing_id) == 0)
    {
      return false;
    }
    else
    {
      unsigned int hand = wielded_items_by_thing_id[thing_id];
      wielded_items_by_thing_id.erase(thing_id);
      wielded_items_by_hand.erase(hand);
      return true;
    }
  }

  bool unwield(unsigned int hand)
  {
    if (wielded_items_by_hand.count(hand) == 0)
    {
      return false;
    }
    else
    {
      ThingId thing_id = wielded_items_by_hand[hand];
      wielded_items_by_hand.erase(hand);
      wielded_items_by_thing_id.erase(thing_id);
      return true;
    }
  }

  void equip(ThingId thing_id, WearLocation location)
  {
    equipped_items_by_thing_id[thing_id] = location;
  }

  bool deequip(ThingId thing_id)
  {
    if (equipped_items_by_thing_id.count(thing_id) == 0)
    {
      return false;
    }
    else
    {
      equipped_items_by_thing_id.erase(thing_id);
      return true;
    }
  }
};

Entity::Entity() :
  Container(), impl(new Impl())
{
}

Entity::Entity(const Entity& original) :
  Container(), impl(new Impl())
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
  // If the thing is YOU, use YOU.
  if (TF.is_player(this))
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
  Container& owner = TF.get_container(get_owner_id());

  if (impl->attributes.get(Attribute::HP) > 0)
  {
    if (owner.is_entity())
    {
      name = owner.get_possessive() + " " + get_description();
    }
    else
    {
      name = "the " + get_description();
    }

    // If the Thing has a proper name, use that.
    if (get_proper_name().empty() == false)
    {
      name += " named " + get_proper_name();
    }
  }
  else
  {
    if (get_proper_name().empty() == false)
    {
      name = get_possessive() + " corpse";
    }
    else
    {
      if (owner.is_entity())
      {
        name = owner.get_possessive() + " dead " + get_description();
      }
      else
      {
        name = "the dead " + get_description();
      }
    }
  }

  return name;
}

std::string Entity::get_indef_name() const
{
  // If the thing is YOU, use YOU.
  if (TF.is_player(this))
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
  ThingId playerId = TF.get_player_id();
  if (get_id() == playerId)
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
    MapTile& tile = TF.get_tile(get_location_id());
    sf::Vector2i coords = tile.get_coords();
    sf::Vector2i check_coords;

    Map& game_map = MF.get(game_map_id);
    bool is_in_bounds = game_map.calc_coords(coords, direction, check_coords);

    if (is_in_bounds)
    {
      MapTile& new_tile = game_map.get_tile(check_coords);
      return new_tile.can_be_traversed_by(*this);
    }
  }
  return false;
}

bool Entity::move_into(ThingId new_location_id)
{
  MapId old_map_id = this->get_map_id();
  bool moveOkay = Thing::move_into(new_location_id);
  if (moveOkay)
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
  return moveOkay;
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

  MapTile& thing_location = TF.get_tile(thing.get_location_id());

  sf::Vector2i thing_coords = thing_location.get_coords();

  return can_see(thing_coords.x, thing_coords.y);
}

bool Entity::can_see(sf::Vector2i coords)
{
  return this->can_see(coords.x, coords.y);
}

bool Entity::can_see(int xTile, int yTile)
{
  // Are we on a map?  Bail out if we aren't.
  if (this->get_map_id() == MapFactory::null_map_id)
  {
    return false;
  }

  // If the coordinates are where we are, then yes, we can indeed see the tile.
  MapTile& tile = TF.get_tile(get_location_id());
  sf::Vector2i tile_coords = tile.get_coords();

  if ((tile_coords.x == xTile) && (tile_coords.y == yTile))
  {
    return true;
  }

  Map& game_map = MF.get(tile.get_map_id());

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
  Container& location = TF.get_container(get_location_id());

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
  float ts2 = ts / 2.0;

  sf::Vector2f location(x * ts, y * ts);
  sf::Vector2f vSW(location.x - ts2, location.y + ts2);
  sf::Vector2f vSE(location.x + ts2, location.y + ts2);
  sf::Vector2f vNW(location.x - ts2, location.y - ts2);
  sf::Vector2f vNE(location.x + ts2, location.y - ts2);

  MapTileType tile_type = impl->map_memory[game_map.get_index(x, y)];
  sf::Vector2u tile_coords = getMapTileTypeTileSheetCoords(tile_type);
  sf::Vector2f texNW = sf::Vector2f(tile_coords.x * ts,
                                    tile_coords.y * ts);

  new_vertex.color = sf::Color::White;
  new_vertex.position = vNW;
  new_vertex.texCoords = texNW;
  vertices.append(new_vertex);

  new_vertex.position = vNE;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts, texNW.y);
  vertices.append(new_vertex);

  new_vertex.position = vSE;
  new_vertex.texCoords = sf::Vector2f(texNW.x + ts, texNW.y + ts);
  vertices.append(new_vertex);

  new_vertex.position = vSW;
  new_vertex.texCoords = sf::Vector2f(texNW.x, texNW.y + ts);
  vertices.append(new_vertex);
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

void Entity::queue_action(Action action)
{
  impl->actions.push_back(action);
}

bool Entity::pending_action()
{
  return !(impl->actions.empty());
}

bool Entity::do_process()
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
  _do_process();

  // Is the entity dead?
  if (impl->attributes.get(Attribute::HP) > 0)
  {
    // If the entity is not the player, perform the AI strategy associated with
    // it.
    if (!TF.is_player(this))
    {
      if (impl->ai_strategy.get() != nullptr)
      impl->ai_strategy->execute();
    }

    // If actions are pending...
    if (!impl->actions.empty())
    {
      Action action = impl->actions.front();
      impl->actions.pop_front();

      unsigned int number_of_things = action.thing_ids.size();

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
        for (ThingId const& id : action.thing_ids)
        {
        success = this->drop(id, action_time);
        if (success)
        {
          impl->busy_counter += action_time;
        }
      }
      break;

      case Action::Type::Eat:
        for (ThingId const& id : action.thing_ids)
        {
          success = this->eat(id, action_time);
          if (success)
          {
            impl->busy_counter += action_time;
          }
        }

      case Action::Type::Pickup:
        for (ThingId const& id : action.thing_ids)
        {
          success = this->pick_up(id, action_time);
          if (success)
          {
            impl->busy_counter += action_time;
          }
        }
        break;

      case Action::Type::Quaff:
        for (ThingId const& id : action.thing_ids)
        {
          success = this->drink(id, action_time);
          if (success)
          {
            impl->busy_counter += action_time;
          }
        }
        break;

      case Action::Type::Store:
        if (isType(&TF.get(action.target), Container))
        {
          for (unsigned int index = 0; index < number_of_things; ++index)
          {
            ThingId thing = action.thing_ids[index];
            success = this->put_into(thing, action.target, action_time);
            if (success)
            {
              impl->busy_counter += action_time;
            }
          }
        }
        else
        {
          the_message_log.add("That target is not a container.");
        }
        break;

      case Action::Type::TakeOut:
        for (ThingId const& id : action.thing_ids)
        {
          success = this->take_out(id, action_time);
          if (success)
          {
            impl->busy_counter += action_time;
          }
        }
        break;

      case Action::Type::Wield:
        if (number_of_things > 1)
        {
          the_message_log.add("NOTE: Only wielding the last item selected.");
        }
        /// @todo Implement wielding using other hands.
        success = this->wield(action.thing_ids[number_of_things - 1],
                              0, action_time);
        if (success)
        {
          impl->busy_counter += action_time;
        }
        break;

      default:
        MINOR_ERROR("Unimplemented action.type %d", action.type);
        break;
      } // end switch (action)
    } // end if (actions pending)
  } // end if (HP > 0)

  return true;
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

bool Entity::is_wielding(ThingId thing_id)
{
  unsigned int dummy;
  return is_wielding(thing_id, dummy);
}

bool Entity::is_wielding(ThingId thing_id, unsigned int& hand)
{
  bool wielding = impl->is_wielding_thing(thing_id, hand);

  return wielding;
}

bool Entity::has_equipped(ThingId thing_id)
{
  WearLocation dummy;
  return has_equipped(thing_id, dummy);
}

bool Entity::has_equipped(ThingId thing_id, WearLocation& location)
{
  bool wearing = impl->is_wearing_thing(thing_id, location);

  return wearing;
}

bool Entity::can_reach(ThingId thing_id)
{
  // Check if it's at our location.
  ThingId our_location_id = this->get_location_id();
  ThingId thing_location_id = TF.get(thing_id).get_location_id();
  if (our_location_id == thing_location_id)
  {
    return true;
  }

  // Check if it's in our inventory.
  if (this->get_inventory().contains(thing_id))
  {
    return true;
  }

  return false;
}

bool Entity::attack(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);

  bool reachable = this->can_reach(thing_id);
  /// @todo deal with Entities in your Inventory -- WTF do you do THEN?

  if (reachable)
  {
    /// @todo Write attack code.
    the_message_log.add("TODO: This is where you would attack something.");
  }

  return false;
}

ActionResult Entity::can_drink(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the thing is within reach.
  if (!this->can_reach(thing_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  Thing& thing = TF.get(thing_id);

  // Check that it is something that contains a liquid.
  if (!thing.is_liquid_carrier())
  {
    return ActionResult::FailureNotLiquidCarrier;
  }

  return ActionResult::Success;
}

bool Entity::drink(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);

  ActionResult drink_try = this->can_drink(thing_id, action_time);

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
    if (this->get_id() == TF.get_player_id())
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

ActionResult Entity::can_drop(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's in our inventory.
  if (!this->get_inventory().contains(thing_id))
  {
    return ActionResult::FailureNotPresent;
  }

  // Check that we're not wielding the item.
  if (this->is_wielding(thing_id))
  {
    return ActionResult::FailureItemWielded;
  }

  /// @todo Check that we're not wearing the item.

  return ActionResult::Success;
}

bool Entity::drop(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  Container& entity_location = TF.get_container(get_location_id());

  ActionResult drop_try = this->can_drop(thing_id, action_time);

  switch (drop_try)
  {
  case ActionResult::Success:
    {
      if (thing.is_movable())
      {
        if (entity_location.can_contain(thing))
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

          message = entity_location.get_name() + " cannot hold " +
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
      if (TF.get_player_id() == this->get_id())
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

ActionResult Entity::can_eat(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the thing is within reach.
  if (!this->can_reach(thing_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  return ActionResult::Success;
}

bool Entity::eat(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);

  ActionResult eat_try = this->can_eat(thing_id, action_time);

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
    if (this->get_id() == TF.get_player_id())
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

ActionResult Entity::can_mix(ThingId thing1_id, ThingId thing2_id,
                             unsigned int& action_time)
{
  action_time = 1;

  // Check that they aren't both the same thing.
  if (thing1_id == thing2_id)
  {
    return ActionResult::FailureCircularReference;
  }

  // Check that neither of them is us.
  if (thing1_id == this->get_id() || thing2_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that both are within reach.
  if (!this->can_reach(thing1_id) || !this->can_reach(thing2_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  /// @todo write Entity::can_mix
  return ActionResult::Success;
}

bool Entity::mix(ThingId thing1_id, ThingId thing2_id, unsigned int& action_time)
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

  // Next: make sure we CAN move!
  if (!can_currently_move())
  {
    message = _YOU_ + choose_verb(" do", " does") +
              " not have the capability of movement.";
    the_message_log.add(message);
    return false;
  }

  // Next: make sure we aren't inside something
  Thing& location = TF.get(get_location_id());
  if (!location.is_maptile())
  {
    message = _YOU_ARE_ + " inside " + location.get_indef_name() +
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
    MapTile& target_tile = TF.get_tile(get_location_id());
    sf::Vector2i coords = target_tile.get_coords();
    int x_offset = get_x_offset(new_direction);
    int y_offset = get_y_offset(new_direction);
    int x_new = coords.x + x_offset;
    int y_new = coords.y + y_offset;
    Map& current_map = MF.get(target_tile.get_map_id());
    sf::Vector2i map_size = current_map.get_size();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
        (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      message = _YOU_ + " can't move there; it is out of bounds!";
      the_message_log.add(message);
      return false;
    }

    MapTile& new_tile = current_map.get_tile(x_new, y_new);
    ThingId new_tile_id = current_map.get_tile_id(x_new, y_new);

    if (new_tile.can_be_traversed_by(*this))
    {
      return move_into(new_tile_id);
    }
    else
    {
      message = _YOU_ARE_ + " stopped by " + new_tile.get_name() + ".";
      the_message_log.add(message);
      return false;
    }
  } // end else if (other direction)
}

ActionResult Entity::can_pick_up(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check if it's already in our inventory.
  if (this->get_inventory().contains(thing_id))
  {
    return ActionResult::FailureAlreadyPresent;
  }

  // Check that it's within reach.
  if (!this->can_reach(thing_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  /// @todo When picking up, check if our inventory is full-up.
  return ActionResult::Success;
}

bool Entity::pick_up(ThingId thing_id, unsigned int& action_time)
{
  std::string message;

  Thing& thing = TF.get(thing_id);

  ActionResult pick_up_try = this->can_pick_up(thing_id, action_time);

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
          if (thing.move_into(*this))
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
        if (TF.get_player_id() == this->get_id())
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

ActionResult Entity::can_put_into(ThingId thing_id, ThingId container_id,
                                  unsigned int& action_time)
{
  action_time = 1;

  // Check that the thing and container aren't the same thing.
  if (thing_id == container_id)
  {
    return ActionResult::FailureCircularReference;
  }

  // Check that the thing and the container are not US!
  if ((thing_id == this->get_id()) || (container_id == this->get_id()))
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the thing's location isn't already the container.
  Thing& thing = TF.get(thing_id);
  if (thing.get_location_id() == container_id)
  {
    return ActionResult::FailureAlreadyPresent;
  }

  // Check that the container is, well, a container!
  if (!TF.get(container_id).is_container())
  {
    return ActionResult::FailureTargetNotAContainer;
  }

  // Check that the thing is within reach.
  if (!this->can_reach(thing_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  // Check that the container is within reach.
  if (!this->can_reach(container_id))
  {
    return ActionResult::FailureContainerOutOfReach;
  }

  /// @todo Make sure the container can hold this thing.
  return ActionResult::Success;
}

bool Entity::put_into(ThingId thing_id, ThingId container_id,
                      unsigned int& action_time)
{
  std::string message;

  ActionResult put_try = this->can_put_into(thing_id, container_id, action_time);
  Thing& thing = TF.get(thing_id);
  Thing& container = TF.get(container_id);

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
        if (!thing.move_into(container_id))
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
      if (TF.get_player_id() == this->get_id())
      {
        if (thing_id == TF.get_player_id())
        {
          /// @todo Possibly allow player to voluntarily enter a container?
          message = "I'm afraid you can't do that.  "
                    "(At least, not in this version...)";
        }
        else if (container_id == TF.get_player_id())
        {
          message = "Store something in yourself?  "
                    "What do you think you are, a drug mule?";
        }
      }
      else
      {
        if (thing_id == TF.get_player_id())
        {
          message = _YOU_TRY_ + " to store " + _YOURSELF_ +
                    "into the " + thing.get_name() +
                    ", which seriously shouldn't happen.";
          MINOR_ERROR("Non-player Entity tried to store self!?");
        }
        else if (container_id == TF.get_player_id())
        {
          message = _YOU_TRY_ + " to store " + thing.get_name() +
                    "into " + _YOURSELF_ +
                    ", which seriously shouldn't happen.";
          MINOR_ERROR("Non-player Entity tried to store into self!?");
        }
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
      if (TF.get_player_id() == this->get_id())
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

ActionResult Entity::can_read(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that the thing is within reach and is not the MapTile we're on.
  if (!this->can_reach(thing_id) && thing_id != this->get_location_id())
  {
    return ActionResult::FailureThingOutOfReach;
  }

  if (0) ///< @todo Intelligence tests for reading.
  {
    return ActionResult::FailureTooStupid;
  }

  return ActionResult::Success;
}

bool Entity::read(ThingId thing_id, unsigned int& action_time)
{
  std::string message;

  Thing& thing = TF.get(thing_id);

  ActionResult read_try = this->can_read(thing_id, action_time);

  switch (read_try)
  {
  case ActionResult::Success:
    {
      if (thing.readable_by(*this))
      {
        switch (thing.perform_action_read_by(*this))
        {
        case ActionResult::SuccessDestroyed:
          if (!thing.is_maptile())
          {
            if (!thing.move_into(TF.limbo_id))
            {
              MAJOR_ERROR("Could not move Thing to Limbo, when trying to "
                          "destroy it after reading");
            }
          }
          else
          {
            MINOR_ERROR("perform_action_read_by returned SuccessDestroyed "
                        "on a MapTile, which can't be destroyed.");
          }
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

ActionResult Entity::can_take_out(ThingId thing_id,
                                  unsigned int& action_time)
{
  Thing& thing = TF.get(thing_id);
  ThingId container_id = thing.get_location_id();

  action_time = 1;

  // Check that the thing isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that the container is not a MapTile or Entity.
  Container& container = TF.get_container(container_id);
  if (container.is_maptile() || container.is_entity())
  {
    return ActionResult::FailureNotInsideContainer;
  }

  // Check that the container is within reach.
  if (!this->can_reach(container_id))
  {
    return ActionResult::FailureContainerOutOfReach;
  }

  return ActionResult::Success;
}


bool Entity::take_out(ThingId thing_id,
                         unsigned int& action_time)
{
  std::string message;

  ActionResult takeout_try = this->can_take_out(thing_id, action_time);
  Thing& thing = TF.get(thing_id);
  Thing& container = TF.get(thing.get_location_id());
  ThingId new_location_id = container.get_location_id();

  switch (takeout_try)
  {
  case ActionResult::Success:
    {
      if (thing.perform_action_take_out())
      {
        if (!thing.move_into(new_location_id))
        {
          MAJOR_ERROR("Could not move Thing out of Container");
        }
        else
        {
          message = _YOU_ + choose_verb(" remove ", "removes ") +
                    thing.get_name() + " from " +
                    container.get_name() + ".";
          the_message_log.add(message);
        }
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (TF.get_player_id() == this->get_id())
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
                container.get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ + " cannot reach " + container.get_name() + ".";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", takeout_try);
    break;
  }

  return false;
}

ActionResult Entity::can_toss(ThingId thing_id,
                              unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's in our inventory.
  if (!this->get_inventory().contains(thing_id))
  {
    return ActionResult::FailureNotPresent;
  }

  /// @todo Check that we're not wearing the item.

  return ActionResult::Success;
}

bool Entity::toss(ThingId thing_id, Direction& direction,
                  unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  ActionResult toss_try = this->can_toss(thing_id, action_time);

  switch (toss_try)
  {
  case ActionResult::Success:
    {
      Container& new_location = TF.get_container(this->get_location_id());
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
      if (TF.get_player_id() == this->get_id())
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

ActionResult Entity::can_deequip(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's already being worn.
  if (!this->has_equipped(thing_id))
  {
    return ActionResult::FailureItemNotEquipped;
  }

  /// @todo Finish Entity::can_deequip code
  return ActionResult::Success;
}

bool Entity::deequip(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  ActionResult deequip_try = this->can_deequip(thing_id, action_time);
  std::string thing_name = thing.get_name();

  message = _YOU_TRY_ + " to take off " + thing_name;
  the_message_log.add(message);

  switch (deequip_try)
  {
    case ActionResult::Success:
      {
        // Get the body part this item is equipped on.
        WearLocation location;
        this->has_equipped(thing_id, location);

        if (thing.perform_action_deequipped_by(*this, location))
        {
          WearLocation location = impl->equipped_items_by_thing_id[thing_id];
          impl->deequip(thing_id);

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

ActionResult Entity::can_equip(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // Check that it's within reach.
  if (!this->can_reach(thing_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  std::string message;
  Thing& thing = TF.get(thing_id);
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

bool Entity::equip(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  ActionResult equip_try = this->can_equip(thing_id, action_time);
  std::string thing_name = thing.get_name();

  switch (equip_try)
  {
    case ActionResult::Success:
      {
        WearLocation location;

        if (thing.perform_action_equipped_by(*this, location))
        {
          impl->equip(thing_id, location);
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
      if (TF.get_player_id() == this->get_id())
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

ActionResult Entity::can_wield(ThingId thing_id,
                               unsigned int hand,
                               unsigned int& action_time)
{
  action_time = 1;

  // GSL: Actually, it CAN be us in this case; wielding self means bare hands.
  if (thing_id == this->get_id())
  {
    return ActionResult::SuccessSelfReference;
  }

  // Check that it's within reach.
  if (!this->can_reach(thing_id))
  {
    return ActionResult::FailureThingOutOfReach;
  }

  return ActionResult::Success;
}

bool Entity::wield(ThingId thing_id,
                   unsigned int hand,
                   unsigned int& action_time)
{
  std::string message;
  std::string bodypart_desc =
    this->get_bodypart_description(BodyPart::Hand, hand);
  Thing& thing = TF.get(thing_id);
  std::string thing_name = thing.get_name();

  // First, check if we're already wielding something.
  ThingId wielded_id;
  bool already_wielding = impl->is_wielding_in(hand, wielded_id);

  // Now, check if the thing we're already wielding is THIS thing.
  if (wielded_id == thing_id)
  {
    message = _YOU_ARE_ + " already wielding " + thing_name + " with " +
              _YOUR_ + " " + bodypart_desc + ".";
    the_message_log.add(message);
    return true;
  }
  else
  {
    // Try to unwield the old item.
    bool unwield_success = true;
    if (already_wielding)
    {
      unwield_success = false;
      Thing& wielded_thing = TF.get(wielded_id);
      if (wielded_thing.perform_action_unwielded_by(*this))
      {
        impl->unwield(wielded_id);
        unwield_success = true;
      }
    }

    if (!unwield_success)
    {
      return false;
    }
  }

  // Try to wield the new item.
  ActionResult wield_try = this->can_wield(thing_id, hand, action_time);

  switch (wield_try)
  {
  case ActionResult::Success:
  case ActionResult::SuccessSwapHands:
    {
      if (thing.perform_action_wielded_by(*this))
      {
        impl->wield(thing_id, hand);
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

// *** PROTECTED METHODS ******************************************************

void Entity::_do_process()
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
  Container& location = TF.get_container(get_location_id());

  if (!location.is_maptile())
  {
    return;
  }

  MapTile& tile = TF.get_tile(get_location_id());
  sf::Vector2i tile_coords = tile.get_coords();
  Map& game_map = MF.get(tile.get_map_id());
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x - 1, y).is_opaque())
            {
              do_recursive_visibility(1, depth + 1, slope_A,
                                      calc_slope(x - 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x - 1, y).is_opaque())
            {
              slope_A = calc_slope(x - 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x + 1, y).is_opaque())
            {
              do_recursive_visibility(2, depth + 1, slope_A,
                                      calc_slope(x + 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x + 1, y).is_opaque())
            {
              slope_A = -calc_slope(x + 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x, y - 1).is_opaque())
            {
              do_recursive_visibility(3, depth + 1, slope_A,
                                      calc_inv_slope(x - 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y - 1).is_opaque())
            {
              slope_A = -calc_inv_slope(x + 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x, y + 1).is_opaque())
            {
              do_recursive_visibility(4, depth + 1, slope_A,
                                      calc_inv_slope(x - 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y + 1).is_opaque())
            {
              slope_A = calc_inv_slope(x + 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x + 1, y).is_opaque())
            {
              do_recursive_visibility(5, depth + 1, slope_A,
                                      calc_slope(x + 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x + 1, y).is_opaque())
            {
              slope_A = calc_slope(x + 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x - 1, y).is_opaque())
            {
              do_recursive_visibility(6, depth + 1, slope_A,
                                      calc_slope(x - 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x - 1, y).is_opaque())
            {
              slope_A = -calc_slope(x - 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x, y + 1).is_opaque())
            {
              do_recursive_visibility(7, depth + 1, slope_A,
                                      calc_inv_slope(x + 0.5, y + 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y + 1).is_opaque())
            {
              slope_A = -calc_inv_slope(x - 0.5, y + 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          if (game_map.get_tile(x, y).is_opaque())
          {
            if (!game_map.get_tile(x, y - 1).is_opaque())
            {
              do_recursive_visibility(8, depth + 1, slope_A,
                                      calc_inv_slope(x + 0.5, y - 0.5, eX, eY));
            }
          }
          else
          {
            if (game_map.get_tile(x, y - 1).is_opaque())
            {
              slope_A = calc_inv_slope(x - 0.5, y - 0.5, eX, eY);
            }
          }
          impl->tile_seen[game_map.get_index(x, y)] = true;
          impl->map_memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
        }
        ++y;
      }
      --y;
      break;
    default:
      MAJOR_ERROR("Octant passed to do_recursive_visibility was %d (not 1 to 8)!", octant);
      break;
  }

  if ((depth < mv) && (!game_map.get_tile(x, y).is_opaque()))
  {
    do_recursive_visibility(octant, depth + 1, slope_A, slope_B);
  }
}
