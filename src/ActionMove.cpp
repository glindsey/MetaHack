#include "stdafx.h"

#include "ActionMove.h"

#include "ActionAttack.h"
#include "GameState.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Map.h"
#include "Service.h"
#include "Thing.h"
#include "ThingId.h"

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
    message += make_string("$you $(cv?don't:doesn't) have the capability of movement.");
    Service<IMessageLog>::get().add(message);
    return Action::StateResult::Failure();
  }

  // Make sure we can move RIGHT NOW.
  if (!subject->can_currently_move())
  {
    message += make_string("$you can't move right now.");
    Service<IMessageLog>::get().add(message);
    return Action::StateResult::Failure();
  }

  // Make sure we're not confined inside another thing.
  if (subject->is_inside_another_thing())
  {
    message += make_string("$you $are inside $0 and $are not going anywhere!",
    { location->get_identifying_string(ArticleChoice::Indefinite) });

    Service<IMessageLog>::get().add(message);
    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

Action::StateResult ActionMove::do_begin_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();

  std::string message;

  auto subject = get_subject();
  ThingId location = subject->get_location();
  MapTile* current_tile = subject->get_maptile();
  Direction new_direction = get_target_direction();

  if (new_direction == Direction::Up)
  {
    /// @todo Write up/down movement code
    message = "Up/down movement is not yet supported!";
    Service<IMessageLog>::get().add(message);
  }
  else if (new_direction == Direction::Down)
  {
    /// @todo Write up/down movement code
    message = "Up/down movement is not yet supported!";
    Service<IMessageLog>::get().add(message);
  }
  else
  {
    // Figure out our target location.
    Vec2i coords = current_tile->get_coords();
    Vec2i offset = (Vec2i)new_direction;
    int x_new = coords.x + offset.x;
    int y_new = coords.y + offset.y;
    Map& current_map = GAME.get_maps().get(subject->get_map_id());
    Vec2i map_size = current_map.get_size();

    // Check boundaries.
    if ((x_new < 0) || (y_new < 0) ||
      (x_new >= map_size.x) || (y_new >= map_size.y))
    {
      message += make_string("$you can't move there; it is out of bounds!");
      Service<IMessageLog>::get().add(message);
    }
    else
    {
      auto& new_tile = current_map.get_tile({ x_new, y_new });
      ThingId new_floor = new_tile.get_tile_contents();

      // See if the tile to move into contains another creature.
      ThingId creature = new_floor->get_inventory().get_entity();
      if (creature != ThingId::Mu())
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
          message += make_string("$you $are stopped by $0 $1.",
          {
            getIndefArt(tile_description),
            tile_description
          });

          Service<IMessageLog>::get().add(message);
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