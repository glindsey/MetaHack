#include "stdafx.h"

#include "ActionMove.h"

#include "ActionAttack.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "Service.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "systems/SystemManager.h"
#include "systems/SystemSpacialRelationships.h"

#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionMove ActionMove::prototype;
  ActionMove::ActionMove() : Action("move", "MOVE", ActionMove::create_) {}
  ActionMove::ActionMove(EntityId subject) : Action(subject, "move", "MOVE") {}
  ActionMove::~ActionMove() {}

  ReasonBool ActionMove::subjectIsCapable(GameState& gameState) const
  {
    auto subject = getSubject();
    bool isMobile = (COMPONENTS.mobility.existsFor(subject) && COMPONENTS.mobility[subject].moveSpeed() > 0);
    std::string reason = isMobile ? "" : "YOU_HAVE_NO_WAY_OF_MOVING"; /// @todo Add translation key
    return { isMobile, reason };      
  }

  std::unordered_set<Trait> const & ActionMove::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbDirection,
      Trait::SubjectCanNotBeInsideAnotherObject
    };

    return traits;
  }

  StateResult ActionMove::doPreBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    // All checks handled in Action by traits.
    return StateResult::Success();
  }

  StateResult ActionMove::doBeginWorkNVI(GameState& gameState, AnyMap& params)
  {
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = getSubject();
    auto position = COMPONENTS.position[subject];
    Direction new_direction = getTargetDirection();

    if (new_direction == Direction::Up)
    {
      /// @todo Write up/down movement code
      putTr("ACTN_NOT_IMPLEMENTED");
    }
    else if (new_direction == Direction::Down)
    {
      /// @todo Write up/down movement code
      putTr("ACTN_NOT_IMPLEMENTED");
    }
    else
    {
      // Figure out our target location.
      IntVec2 coords = position.coords();
      MapId map = position.map();
      IntVec2 offset = (IntVec2)new_direction;
      int x_new = coords.x + offset.x;
      int y_new = coords.y + offset.y;
      IntVec2 map_size = map->getSize();

      // Check boundaries.
      if ((x_new < 0) || (y_new < 0) ||
        (x_new >= map_size.x) || (y_new >= map_size.y))
      {
        putTr("YOU_CANT_VERB_THERE");
     }
      else
      {
        auto& new_tile = map->getTile({ x_new, y_new });
        EntityId new_floor = new_tile.getTileContents();

        // See if the tile to move into contains another creature.
        EntityId creature = COMPONENTS.inventory[new_floor].getEntity();
        if (creature != EntityId::Mu())
        {
          /// @todo Setting choosing whether auto-attack is on.
          /// @todo Only attack hostiles.
          std::unique_ptr<ActionAttack> action_attack{ new ActionAttack(subject) };
          action_attack->setTarget(new_direction);

          subject->queueAction(std::move(action_attack));

          result = StateResult::Success();
        }
        else
        {
          if (new_tile.canBeTraversedBy(subject))
          {
            /// @todo Figure out elapsed movement time.
            result.success = SYSTEMS.spacial().moveEntityInto(subject, new_floor);
            result.elapsed_time = 1;
          }
          else
          {
            std::string tile_description = new_tile.getDisplayName();
            putMsg(makeTr("YOU_ARE_STOPPED_BY",
            {
              getIndefArt(tile_description),
              tile_description
            }));
          }
        } // end else if (tile does not contain creature)
      } // end else if (not out of bounds)
    } // end else if (other direction)

    return result;
  }

  StateResult ActionMove::doFinishWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionMove::doAbortWorkNVI(GameState& gameState, AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

