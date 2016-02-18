#include "ActionAttack.h"
#include "GameState.h"
#include "Map.h"
#include "MapFactory.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_BOILERPLATE(ActionAttack)

Action::StateResult ActionAttack::do_prebegin_work_(AnyMap& params)
{
  std::string message;

  auto subject = get_subject();
  auto location = subject->get_location();
  MapTile* current_tile = subject->get_maptile();
  auto new_direction = get_target_direction();

  // Make sure we're not in limbo!
  if ((location == ThingManager::get_mu()) || (current_tile == nullptr))
  {
    message = YOU + " can't attack anything because " + YOU_DO + " not exist physically!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  if (!IS_PLAYER)
  {
    /// @todo Clean this message up.
    message = YOU_TRY_TO("attack") + str(new_direction) + ".";
    the_message_log.add(message);
    message = "But ";
  }
  else
  {
    message = "";
  }

  // Make sure we CAN attack.
  if (!subject->get_intrinsic<bool>("can_attack", false))
  {
    message += YOU + CV(" don't", " doesn't") + " have any way to attack things.";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Make sure we can move RIGHT NOW.
  if (!subject->can_currently_move())
  {
    message += YOU + " can't move right now.";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Check if we're attacking ourself.
  if (new_direction == Direction::Self)
  {
    /// @todo Allow attacking yourself!
    message = YOU + " wisely" + CV(" refrain", " refrains") + " from pummelling " + YOURSELF + ".";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  // Make sure we're not confined inside another thing.
  /// @todo Allow for attacking when swallowed!
  if (subject->is_inside_another_thing())
  {
    message += YOU_ARE + " inside " + location->get_identifying_string(false) + " and " + ARE + " not going anywhere!";

    the_message_log.add(message);
    return StateResult::Failure();
  }

  if (new_direction == Direction::Up)
  {
    /// @todo Write up/down attack code
    message = "Attacking the ceiling is not yet supported!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  if (new_direction == Direction::Down)
  {
    /// @todo Write up/down attack code
    message = "Attacking the floor is not yet supported!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  return StateResult::Success();
}

Action::StateResult ActionAttack::do_begin_work_(AnyMap& params)
{
  std::string message;

  auto subject = get_subject();
  auto location = subject->get_location();
  MapTile* current_tile = subject->get_maptile();
  auto new_direction = get_target_direction();

  bool success;
  unsigned int action_time;

  // Figure out our target location.
  sf::Vector2i coords = current_tile->get_coords();
  sf::Vector2i offset = get_vector(new_direction);
  int x_new = coords.x + offset.x;
  int y_new = coords.y + offset.y;
  Map& current_map = GAME.get_map_factory().get(subject->get_map_id());
  sf::Vector2i map_size = current_map.get_size();

  // Check boundaries.
  if ((x_new < 0) || (y_new < 0) ||
      (x_new >= map_size.x) || (y_new >= map_size.y))
  {
    message += YOU + " can't attack there; it is out of bounds!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  auto& new_tile = current_map.get_tile(x_new, y_new);
  ThingRef new_floor = new_tile.get_tile_contents();

  // See if the tile to move into contains another creature.
  ThingRef creature = new_floor->get_inventory().get_entity();
  if (creature == ThingManager::get_mu())
  {
    /// @todo Deal with attacking other stuff, MapTiles, etc.
    message = "Attacking non-entity things is not yet supported!";
    the_message_log.add(message);
    return StateResult::Failure();
  }

  success = subject->do_attack(creature, action_time);

  return{ success, action_time };
}

Action::StateResult ActionAttack::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionAttack::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}