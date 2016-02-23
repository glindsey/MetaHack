#include "ActionMove.h"

#include "ActionAttack.h"
#include "GameState.h"
#include "Map.h"
#include "Thing.h"
#include "ThingRef.h"

ACTION_SRC_BOILERPLATE(ActionMove, "move", "move")

Action::StateResult ActionMove::do_prebegin_work_(AnyMap& params)
{
  std::string message;

  auto subject = get_subject();
  auto location = subject->get_location();
  auto new_direction = get_target_direction();

  if (!IS_PLAYER)
  {
    print_message_try_();

    message = "But ";
  }
  else
  {
    message = "";
  }

  // Make sure we CAN move.
  if (!subject->get_intrinsic<bool>("can_move", false))
  {
    message += YOU + CV(" don't", " doesn't") + " have the capability of movement.";
    the_message_log.add(message);
    return Action::StateResult::Failure();
  }

  // Make sure we can move RIGHT NOW.
  if (!subject->can_currently_move())
  {
    message += YOU + " can't move right now.";
    the_message_log.add(message);
    return Action::StateResult::Failure();
  }

  // Make sure we're not confined inside another thing.
  if (subject->is_inside_another_thing())
  {
    message += YOU_ARE + " inside " + location->get_identifying_string(false) + " and " + ARE + " not going anywhere!";

    the_message_log.add(message);
    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionMove::do_begin_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();

  std::string message;

  auto subject = get_subject();
  ThingRef location = subject->get_location();
  MapTile* current_tile = subject->get_maptile();
  Direction new_direction = get_target_direction();

  if (new_direction == Direction::Up)
  {
    /// @todo Write up/down movement code
    message = "Up/down movement is not yet supported!";
    the_message_log.add(message);
  }
  else if (new_direction == Direction::Down)
  {
    /// @todo Write up/down movement code
    message = "Up/down movement is not yet supported!";
    the_message_log.add(message);
  }
  else
  {
    // Figure out our target location.
    sf::Vector2i coords = current_tile->get_coords();
    sf::Vector2i offset = (sf::Vector2i)new_direction;
    int x_new = coords.x + offset.x;
    int y_new = coords.y + offset.y;
    Map& current_map = GAME.get_map_factory().get(subject->get_map_id());
    sf::Vector2i map_size = current_map.get_size();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
        (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      message += YOU + " can't move there; it is out of bounds!";
      the_message_log.add(message);
    }
    else
    {
      auto& new_tile = current_map.get_tile(x_new, y_new);
      ThingRef new_floor = new_tile.get_tile_contents();

      // See if the tile to move into contains another creature.
      ThingRef creature = new_floor->get_inventory().get_entity();
      if (creature != ThingManager::get_mu())
      {
        /// @todo Setting choosing whether auto-attack is on.
        /// @todo Only attack hostiles.
        /// @todo Change this to pushing an attack action onto the subject's queue
        ///       instead of just segueing right into the attack.
        std::unique_ptr<ActionAttack> action_attack{ new ActionAttack(subject) };
        action_attack->set_target(new_direction);

        subject->queue_action(std::move(action_attack));

        result = StateResult::Success();
      }
      else
      {
        if (new_tile.can_be_traversed_by(subject))
        {
          /// @todo Figure out elapsed movement time.
          result.success = subject->move_into(new_floor);
          result.elapsed_time = 1;
        }
        else
        {
          std::string tile_description = new_tile.get_display_name();
          message += YOU_ARE + " stopped by " +
            getIndefArt(tile_description) + " " +
            tile_description + ".";
          the_message_log.add(message);
        }
      } // end else if (tile does not contain creature)
    } // end else if (not out of bounds)
  } // end else if (other direction)

  return result;
}

Action::StateResult ActionMove::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionMove::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}