#include "stdafx.h"

#include "ActionMove.h"

#include "ActionAttack.h"
#include "game/GameState.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "map/Map.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionMove ActionMove::prototype;
  ActionMove::ActionMove() : Action("move", "MOVE", ActionMove::create_) {}
  ActionMove::ActionMove(EntityId subject) : Action(subject, "move", "MOVE") {}
  ActionMove::~ActionMove() {}

  std::unordered_set<Trait> const & ActionMove::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbDirection,
      Trait::SubjectCanNotBeInsideAnotherObject
    };

    return traits;
  }

  StateResult ActionMove::do_prebegin_work_(AnyMap& params)
  {
    // All checks handled in Action by traits.
    return StateResult::Success();
  }

  StateResult ActionMove::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = get_subject();
    EntityId location = subject->getLocation();
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
      IntVec2 coords = current_tile->get_coords();
      IntVec2 offset = (IntVec2)new_direction;
      int x_new = coords.x + offset.x;
      int y_new = coords.y + offset.y;
      Map& current_map = GAME.get_maps().get(subject->get_map_id());
      IntVec2 map_size = current_map.getSize();

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
        EntityId new_floor = new_tile.get_tile_contents();

        // See if the tile to move into contains another creature.
        EntityId creature = new_floor->get_inventory().getEntity();
        if (creature != EntityId::Mu())
        {
          /// @todo Setting choosing whether auto-attack is on.
          /// @todo Only attack hostiles.
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

  StateResult ActionMove::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionMove::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

