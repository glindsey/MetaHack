#include "stdafx.h"

#include "ActionTurn.h"

#include "ActionAttack.h"
#include "GameState.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Map.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

namespace Actions
{
  ActionTurn ActionTurn::prototype;
  ActionTurn::ActionTurn() : Action("turn", "TURN", ActionTurn::create_) {}
  ActionTurn::ActionTurn(EntityId subject) : Action(subject, "turn", "TURN") {}
  ActionTurn::~ActionTurn() {}

  std::unordered_set<Trait> const & ActionTurn::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionTurn::do_prebegin_work_(AnyMap& params)
  {
    std::string message;

    auto subject = get_subject();
    auto location = subject->getLocation();
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
    /// @todo Split moving/turning up? It seems reasonable that a creature
    ///       might be able to swivel in place without being able to move.
    if (!subject->get_intrinsic<bool>("can_move", false))
    {
      message += YOU + CV(" don't", " doesn't") + " have the capability of movement.";
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    // Make sure we can move RIGHT NOW.
    if (!subject->can_currently_move())
    {
      message += YOU + " can't move right now.";
      Service<IMessageLog>::get().add(message);
      return StateResult::Failure();
    }

    return StateResult::Success();
  }

  /// @todo Implement me.
  StateResult ActionTurn::do_begin_work_(AnyMap& params)
  {
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = get_subject();
    EntityId location = subject->getLocation();
    MapTile* current_tile = subject->get_maptile();
    Direction new_direction = get_target_direction();

    if ((new_direction != Direction::Up) &&
      (new_direction != Direction::Down))
    {
      /// @todo Change facing direction.
      result = StateResult::Success();
    } // end else if (other direction)

    return result;
  }

  StateResult ActionTurn::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTurn::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}
