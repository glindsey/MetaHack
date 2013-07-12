#include "Entity.h"

#include <algorithm>
#include <cmath>
#include <deque>
#include <sstream>

#include "App.h"
#include "ConfigSettings.h"
#include "ErrorHandler.h"
#include "Gender.h"
#include "Inventory.h"
#include "IsType.h"
#include "Map.h"
#include "MapFactory.h"
#include "MapTile.h"
#include "MathUtils.h"
#include "MessageLog.h"
#include "ThingFactory.h"

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

  /// Direction the entity is facing.
  Direction direction = Direction::South;

  /// Entity's memory of map tiles.
  std::vector<MapTileType> memory;

  /// Bitset for seen tiles.
  boost::dynamic_bitset<> tile_seen;

  /// Queue of actions to be performed.
  std::deque<Action> actions;

};

Entity::Entity()
  : Thing(), impl(new Impl())
{

}

Entity::Entity(const Entity& original) :
  Thing(), impl(new Impl())
{
  impl->attributes = original.get_attributes();
  impl->busy_counter = original.get_busy_counter();
  impl->gender = original.get_gender();
  impl->direction = original.get_facing_direction();

  // Memory, seen tiles, and actions are not copied over.
}

Entity::~Entity()
{
  //dtor
}

sf::Vector2u Entity::get_tile_sheet_coords(int frame) const
{
  int x_pos = get_appropriate_4way_tile(this->get_facing_direction());
  return sf::Vector2u(x_pos, 3);  // The "unknown directional" tile
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

void Entity::set_facing_direction(Direction d)
{
  impl->direction = d;
}

Direction Entity::get_facing_direction() const
{
  return impl->direction;
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
        // TODO: save old map memory.
      }
      impl->memory.clear();
      if (new_map_id != MapFactory::null_map_id)
      {
        Map& new_map = MF.get(new_map_id);
        sf::Vector2i new_map_size = new_map.get_size();
        impl->memory.resize(new_map_size.x * new_map_size.y);
        // TODO: load new map memory if it exists somewhere.
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

  // Return seen data.
  return impl->tile_seen[game_map.get_index(xTile, yTile)];
}

void Entity::do_recursive_visibility(int octant,
                                   int depth,
                                   float slope_A,
                                   float slope_B)
{
  int x = 0;
  int y = 0;

  // Are we on a map?  Bail out if we aren't.
  if (!isType(&TF.get(get_location_id()), MapTile))
  {
    return;
  }

  MapTile& tile = TF.get_tile(get_location_id());
  sf::Vector2i tile_coords = tile.get_coords();
  Map& game_map = MF.get(tile.get_map_id());
  int eX = tile_coords.x;
  int eY = tile_coords.y;

  // TODO: make these globals or something, I dunno, whatever
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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
          impl->memory[game_map.get_index(x, y)] = game_map.get_tile(x, y).get_type();
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


void Entity::find_seen_tiles()
{
  static MapId lastId = static_cast<MapId>(0);
  sf::Clock elapsed;

  elapsed.restart();

  // Size and clear the "tile seen" bitset.
  if (!isType(&TF.get(get_location_id()), MapTile))
  {
    impl->tile_seen.clear();
  }
  else
  {
    MapId ourId = TF.get_tile(get_location_id()).get_map_id();

    if (ourId != lastId)
    {
      lastId = ourId;
      Map& game_map = MF.get(ourId);
      sf::Vector2i mapSize = game_map.get_size();
      impl->tile_seen.resize(mapSize.x * mapSize.y);
    }

    impl->tile_seen.reset();
  }

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
  return impl->memory[game_map.get_index(x, y)];
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

  MapTileType tile_type = impl->memory[game_map.get_index(x, y)];
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

  // If no pending actions, just return.
  if (impl->actions.empty())
  {
    return true;
  }

  Action action = impl->actions.front();
  impl->actions.pop_front();

  unsigned int number_of_things = action.thing_ids.size();

  switch (action.type)
  {
  case Action::Type::Wait:
    success = this->move(Direction::Self, false, action_time);
    if (success)
    {
      impl->busy_counter += action_time;
    }
    break;

  case Action::Type::Move:
    success = this->move(action.move_info.direction, true, action_time);
    if (success)
    {
      impl->busy_counter += action_time;
    }

    // If player is on a map, update cursor location to be at the player.
    //if (isType(&TF.get(player.get_location_id()), MapTile))
    //{
    //  impl->cursor_coords = TF.get_tile(player.get_location_id()).get_coords();
    //}
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
    if (number_of_things > 1)
    {
      ThingId target_id = action.thing_ids[number_of_things - 1];
      for (unsigned int index = 0; index < number_of_things - 1; ++index)
      {
        success = this->put_into(action.thing_ids[index],
                                 target_id, action_time);
        if (success)
        {
          impl->busy_counter += action_time;
        }
      }
    }
    else
    {
      the_message_log.add("You need to select at least two items, "
                          "the last being the destination of all the others.");
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

  default:
    MINOR_ERROR("Unimplemented action.type %d", action.type);
    break;
  }

  return true;
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
    if (thing.can_be_drank_by(*this))
    {
      message = _YOU_ + " drink " + thing.get_name();
      the_message_log.add(message);

      if (thing.do_action_drank_by(*this))
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
      // TODO: special message if we're a liquid-based organism
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

  return ActionResult::Success;
}

bool Entity::drop(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  Thing& entity_location = TF.get(get_location_id());

  ActionResult drop_try = this->can_drop(thing_id, action_time);

  switch (drop_try)
  {
  case ActionResult::Success:
    {
      if (thing.can_be_moved())
      {
        if (entity_location.can_contain(thing))
        {
          if (thing.do_action_dropped_by(*this))
          {
            if (thing.move_into(entity_location))
            {
              message = _YOU_ + choose_verb(" drop ", " drops ") +
                        thing.get_name() + ".";
              the_message_log.add(message);
              return true;
            }
            else
            {
              MAJOR_ERROR("Could not move Thing even though "
                          "can_drop returned Success");
              break;
            }
          }
          else // Drop failed
          {
            // do_action_dropped_by() will print any relevant messages
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

  switch (eat_try)
  {
  case ActionResult::Success:
    if (thing.can_be_eaten_by(*this))
    {
      if (thing.do_action_eaten_by(*this))
      {
        return true;
      }
    }
    else
    {
      message = _YOU_TRY_ + " to eat " + thing.get_name() + ".";
      the_message_log.add(message);

      message = _YOU_ + " can't eat that!";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureSelfReference:
    if (this->get_id() == TF.get_player_id())
    {
      message = _YOU_TRY_ + " to eat " + thing.get_name() + ".";
      the_message_log.add(message);

      // TODO: special message if we're a liquid-based organism
      message = "But you really aren't that tasty, so you stop.";
      the_message_log.add(message);
    }
    else
    {
      message = _YOU_TRY_ + " to eat " + _YOURSELF_ +
                ", which seriously shouldn't happen.";
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

ActionResult Entity::can_fire(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // TODO: check that the Thing is in our inventory

  // TODO: write me
  return ActionResult::Success;
}

bool Entity::fire(ThingId thing_id, Direction& direction,
                  unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);

  ActionResult fire_try = this->can_fire(thing_id, action_time);

  switch (fire_try)
  {
  case ActionResult::Success:
    if (thing.can_be_fired_by(*this))
    {
      if (thing.do_action_fired_by(*this, direction))
      {
        // TODO: write me
        return true;
      }
    }
    else
    {
      message = _YOU_TRY_ + " to fire " + thing.get_name();
      the_message_log.add(message);

      message = _YOU_ + " cannot fire that.";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureNotEnoughHands:
    message = _YOU_TRY_ + " to fire " + thing.get_name();
    the_message_log.add(message);

    message = _YOU_ + choose_verb(" lack", " lacks") +
              " the free " + this->get_bodypart_plural(BodyPart::Hand) +
              " to fire things!";
    the_message_log.add(message);
    break;

  case ActionResult::FailureSelfReference:
    if (TF.get_player_id() == this->get_id())
    {
      message = "You aren't your own boss, "
                "so you have no authority to fire yourself.";
    }
    else
    {
      message = _YOU_TRY_ +
                " to fire " + _YOURSELF_ +
                ", which seriously shouldn't happen.";
      MINOR_ERROR("Non-player Entity tried to fire self!?");
    }
    the_message_log.add(message);
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", fire_try);
    break;
  }

  return false;
}

ActionResult Entity::can_mix(ThingId thing1_id, ThingId thing2_id,
                             unsigned int& action_time)
{
  action_time = 1;

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

  // TODO: write me
  return ActionResult::Success;
}

bool Entity::mix(ThingId thing1_id, ThingId thing2_id, unsigned int& action_time)
{
  // TODO: write me
  return false;
}

bool Entity::move(Direction direction, bool turn, unsigned int& action_time)
{
  // TODO: update action time

  std::string message;

  // First: make sure we aren't inside something
  Thing& location = TF.get(get_location_id());
  if (!isType(&location, MapTile))
  {
    message = _YOU_ARE_ + " inside " + location.get_indef_name() +
              " and " + _ARE_ + " not going anywhere!";

    the_message_log.add(message);
    return false;
  }
  else
  {
    // Next: check direction.
    if (direction == Direction::Self)
    {
      message = _YOU_ + " successfully" + choose_verb(" stay", " stays") +
                " where " + get_subject_pronoun() + _ARE_ + ".";
      the_message_log.add(message);
      return true;
    }
    else if (direction == Direction::Up)
    {
      //TODO: write me!
      message = "Up/down movement is not yet supported!";
      the_message_log.add(message);
      return false;
    }
    else if (direction == Direction::Down)
    {
      //TODO: write me!
      message = "Up/down movement is not yet supported!";
      the_message_log.add(message);
      return false;
    }
    else
    {
      bool moveToNewSquare = true;
      if (turn)
      {
        switch (direction)
        {
        case Direction::North:
          moveToNewSquare = (impl->direction == Direction::North);
          break;
        case Direction::Northeast:
          moveToNewSquare = ((impl->direction == Direction::North) ||
                             (impl->direction == Direction::East));
          break;
        case Direction::East:
          moveToNewSquare = (impl->direction == Direction::East);
          break;
        case Direction::Southeast:
          moveToNewSquare = ((impl->direction == Direction::South) ||
                             (impl->direction == Direction::East));
          break;
        case Direction::South:
          moveToNewSquare = (impl->direction == Direction::South);
          break;
        case Direction::Southwest:
          moveToNewSquare = ((impl->direction == Direction::South) ||
                             (impl->direction == Direction::West));
          break;
        case Direction::West:
          moveToNewSquare = (impl->direction == Direction::West);
          break;
        case Direction::Northwest:
          moveToNewSquare = ((impl->direction == Direction::North) ||
                             (impl->direction == Direction::West));
          break;
        default:
          /// This should not happen.
          break;
        }

        // Update new direction based on old direction.
        impl->direction = update_direction(impl->direction, direction);
      }

      if (moveToNewSquare)
      {
        // Figure out our target location.
        MapTile& target_tile = TF.get_tile(get_location_id());
        sf::Vector2i coords = target_tile.get_coords();
        int xOffset = get_x_offset(direction);
        int yOffset = get_y_offset(direction);
        int xNew = coords.x + xOffset;
        int yNew = coords.y + yOffset;
        Map& currentMap = MF.get(target_tile.get_map_id());
        sf::Vector2i mapSize = currentMap.get_size();

        // Check boundaries.
        if ((xNew < 0) || (yNew < 0) ||
            (xNew >= mapSize.x) || (yNew >= mapSize.y))
        {
          message = _YOU_ + " can't move there; it is out of bounds!";
          the_message_log.add(message);
          return false;
        }

        MapTile& newTile = currentMap.get_tile(xNew, yNew);
        ThingId newTileId = currentMap.get_tile_id(xNew, yNew);

        if (newTile.can_be_traversed_by(*this))
        {
          return move_into(newTileId);
        }
        else
        {
          message = _YOU_ARE_ + " stopped by " + newTile.get_name() + ".";
          the_message_log.add(message);
          return false;
        }
      } // end if (moveToNewSquare)
      else
      {
        return true;
      }
    } // end else if (other direction)
  } // end else (on map)
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

  // TODO: Check if our inventory is full-up.
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
      if (thing.can_be_moved())
      {
        if (thing.do_action_picked_up_by(*this))
        {
          if (thing.move_into(*this))
          {
            message = _YOU_ + choose_verb(" pick", " picks") + " up " +
                      thing.get_name() + ".";
            the_message_log.add(message);
            return true;
          }
          else // could not add to inventory
          {
            MAJOR_ERROR("Could not move Thing even though "
                        "can_pick_up returned Success");
            break;
          }
        }
        else // do_action_picked_up_by(*this) returned false
        {
          // do_action_picked_up_by() will print any relevant messages
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

  // Check that the thing isn't US!
  if (thing_id == this->get_id())
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
  Thing& container = TF.get(container_id);
  if (container.get_inventory_size() == 0)
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

  // TODO: Make sure the container can hold this thing.
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
      if (thing.can_be_put_into(container))
      {
        if (thing.do_action_put_into(container))
        {
          if (!thing.move_into(container_id))
          {
            MAJOR_ERROR("Could not move Thing into Container");
          }
          else
          {
            message = _YOU_ + choose_verb(" place ", "places ") +
                      thing.get_name() + " into " +
                      container.get_name() + ".";
            the_message_log.add(message);
            return true;
          }
        }
      }
      else
      {
        message = _YOU_TRY_ + " to place " + thing.get_name() + " into " +
                  container.get_name() + ".";
        the_message_log.add(message);

        message = thing.get_name() + " cannot be put into " +
                  container.get_name();
        the_message_log.add(message);
      }
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (TF.get_player_id() == this->get_id())
      {
        // TODO: allow this?
        message = "I'm afraid you can't do that.  "
                  "(At least, not in this version...)";
      }
      else
      {
        message = _YOU_TRY_ + " to store " + _YOURSELF_ +
                  ", which seriously shouldn't happen.";
        MINOR_ERROR("Non-player Entity tried to store self!?");
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

  if (0) // TODO: intelligence tests
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
      if (thing.can_be_read_by(*this))
      {
        switch (thing.do_action_read_by(*this))
        {
        case ActionResult::SuccessDestroyed:
          if (!isType(&thing, MapTile))
          {
            if (!thing.move_into(TF.limbo_id))
            {
              MAJOR_ERROR("Could not move Thing to Limbo, when trying to "
                          "destroy it after reading");
            }
          }
          else
          {
            MINOR_ERROR("do_action_read_by returned SuccessDestroyed "
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
  Thing& container = TF.get(container_id);
  if (isType(&container, MapTile) || (isType(&container, Entity)))
  {
    return ActionResult::FailureNotInsideContainer;
  }

  // Check that the container is within reach.
  if (!this->can_reach(container_id))
  {
    return ActionResult::FailureContainerOutOfReach;
  }

  // TODO: write me
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
      if (thing.can_be_taken_out())
      {
        if (thing.do_action_take_out())
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
    }
    break;

  case ActionResult::FailureSelfReference:
    {
      if (TF.get_player_id() == this->get_id())
      {
        // TODO: allow this?
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
      Thing& new_location = TF.get(this->get_location_id());
      if (thing.can_be_moved())
      {
        if (thing.can_be_thrown_by(*this))
        {
          if (thing.do_action_thrown_by(*this, direction))
          {
            if (thing.move_into(new_location))
            {
              message = _YOU_ + choose_verb(" throw ", " throws ") +
                        thing.get_name();
              the_message_log.add(message);

              // TODO: set thing's direction and velocity
              return true;
            }
            else
            {
              MAJOR_ERROR("Could not move Thing even though "
                          "can_toss returned Success");
            }
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

  case ActionResult::FailureNotPresent:
    {
      std::string message;

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

ActionResult Entity::can_wear(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // Check that it isn't US!
  if (thing_id == this->get_id())
  {
    return ActionResult::FailureSelfReference;
  }

  // TODO: Check that the thing is in our inventory.

  // TODO: write me
  return ActionResult::Success;
}

bool Entity::wear(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  ActionResult wear_try = this->can_wear(thing_id, action_time);

  switch (wear_try)
  {
    case ActionResult::Success:
      {
        if (thing.can_be_equipped_on(*this))
        {
          if (thing.do_action_equipped_onto(*this))
          {
            // TODO: set worn item?
            return true;
          }
        }
        else
        {
          message = _YOU_TRY_ + " to wear" + thing.get_name();
          the_message_log.add(message);

          message = _YOU_ + " cannot wear " + thing.get_name();
          the_message_log.add(message);
        }
      }
      break;

    default:
      MINOR_ERROR("Unknown ActionResult %d", wear_try);
      break;
  }

  return false;
}

ActionResult Entity::can_wield(ThingId thing_id, unsigned int& action_time)
{
  action_time = 1;

  // GSL: Actually, it CAN be us in this case; wielding self means bare hands.
  if (thing_id == this->get_id())
  {
    return ActionResult::SuccessSelfReference;
  }

  // TODO: Check that the thing is in our inventory.

  return ActionResult::Success;
}

bool Entity::wield(ThingId thing_id, unsigned int& action_time)
{
  std::string message;
  Thing& thing = TF.get(thing_id);
  ActionResult wield_try = this->can_wield(thing_id, action_time);

  switch (wield_try)
  {
  case ActionResult::Success:
    {
      if (thing.can_be_wielded_by(*this))
      {
        if (thing.do_action_wielded_by(*this))
        {
          // TODO: set wielded item?

          message = _YOU_ARE_ + " now wielding " + thing.get_name() + ".";
          the_message_log.add(message);
          return true;
        }
      }
    }
    break;

  case ActionResult::SuccessSelfReference:
    {
      // TODO: unwield everything
      message = _YOU_ARE_ + "no longer wielding any weapons.";
      the_message_log.add(message);
    }
    break;

  case ActionResult::FailureNotEnoughHands:
    {
      message = _YOU_TRY_ + " to wield " + thing.get_name();
      the_message_log.add(message);

      message = _YOU_ + choose_verb(" don't", " doesn't") +
                " have enough free " +
                this->get_bodypart_plural(BodyPart::Hand) +
                " to wield " + thing.get_indef_name() + ".";
      the_message_log.add(message);
    }
    break;

  default:
    MINOR_ERROR("Unknown ActionResult %d", wield_try);
    break;
  }
  return false;
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
