#include "stdafx.h"

#include "ActionMove.h"

#include "ActionAttack.h"
#include "components/ComponentManager.h"
#include "game/GameState.h"
#include "lua/LuaObject.h"
#include "map/Map.h"
#include "maptile/MapTile.h"
#include "services/Service.h"
#include "services/IMessageLog.h"
#include "services/IStrings.h"
#include "systems/Manager.h"
#include "systems/SystemDirector.h"
#include "systems/SystemNarrator.h"
#include "systems/SystemGeometry.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionMove ActionMove::prototype;
  ActionMove::ActionMove() : Action("MOVE", ActionMove::create_) {}
  ActionMove::ActionMove(EntityId subject) : Action(subject, "MOVE") {}
  ActionMove::~ActionMove() {}

  ReasonBool ActionMove::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isMobile = (components.mobility.existsFor(subject) && components.mobility.of(subject).moveSpeed() > 0);
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

  StateResult ActionMove::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // All checks handled in Action by traits.
    return StateResult::Success();
  }

  /// @todo Should have a call to Lua::doReflexiveAction() in here somewhere.
  StateResult ActionMove::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    auto& narrator = systems.narrator();
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = getSubject();
    auto position = components.position.of(subject);
    Direction new_direction = getTargetDirection();

    if (new_direction == Direction::Up)
    {
      /// @todo Write up/down movement code
      putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    }
    else if (new_direction == Direction::Down)
    {
      /// @todo Write up/down movement code
      putMsg(tr("ACTN_NOT_IMPLEMENTED"));
    }
    else
    {
      // Figure out our target location.
      IntVec2 coords = position.coords();
      MapID map = position.map();
      IntVec2 offset = (IntVec2)new_direction;
      int x_new = coords.x + offset.x;
      int y_new = coords.y + offset.y;
      IntVec2 map_size = gameState.maps().get(map).getSize();

      // Check boundaries.
      if ((x_new < 0) || (y_new < 0) ||
        (x_new >= map_size.x) || (y_new >= map_size.y))
      {
        putMsg(narrator.makeTr("YOU_CANT_VERB_THERE", arguments));
      }
      else
      {
        auto& new_tile = gameState.maps().get(map).getTile({ x_new, y_new });
        EntityId new_floor = new_tile.getTileSpace();

        // See if the tile to move into contains another creature.
        EntityId creature = components.inventory.of(new_floor).getEntity();
        if (creature != EntityId::Mu())
        {
          /// @todo Setting choosing whether auto-attack is on.
          /// @todo Only attack hostiles.
          std::unique_ptr<ActionAttack> action_attack{ new ActionAttack(subject) };
          action_attack->setTarget(new_direction);

          systems.director().queueEntityAction(subject, std::move(action_attack));

          result = StateResult::Success();
        }
        else
        {
          if (new_tile.canBeTraversedBy(subject))
          {
            /// @todo Figure out elapsed movement time.
            result.success = systems.geometry().moveEntityInto(subject, new_floor);
            result.elapsed_time = 1;
          }
          else
          {
            std::string tile_description = new_tile.getDisplayName();
            arguments["a_tile"] = getIndefArt(tile_description) + " " + tile_description;
            putMsg(narrator.makeTr("YOU_ARE_STOPPED_BY", arguments));
          }
        } // end else if (tile does not contain creature)
      } // end else if (not out of bounds)
    } // end else if (other direction)

    return result;
  }

  StateResult ActionMove::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionMove::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
} // end namespace

