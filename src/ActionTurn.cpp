#include "stdafx.h"

#include "ActionTurn.h"

#include "ActionAttack.h"
#include "GameState.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Map.h"
#include "Service.h"
#include "Thing.h"
#include "ThingId.h"

ACTION_SRC_BOILERPLATE(ActionTurn, "turn", "turn")

Action::StateResult ActionTurn::do_prebegin_work_(AnyMap& params)
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
  /// @todo Split moving/turning up? It seems reasonable that a creature
  ///       might be able to swivel in place without being able to move.
  if (!subject->get_intrinsic<bool>("can_move", false))
  {
    message += YOU + CV(" don't", " doesn't") + " have the capability of movement.";
    Service<IMessageLog>::get().add(message);
    return Action::StateResult::Failure();
  }

  // Make sure we can move RIGHT NOW.
  if (!subject->can_currently_move())
  {
    message += YOU + " can't move right now.";
    Service<IMessageLog>::get().add(message);
    return Action::StateResult::Failure();
  }

  return Action::StateResult::Success();
}

/// @todo Implement me.
Action::StateResult ActionTurn::do_begin_work_(AnyMap& params)
{
  StateResult result = StateResult::Failure();

  std::string message;

  auto subject = get_subject();
  ThingId location = subject->get_location();
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

Action::StateResult ActionTurn::do_finish_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}

Action::StateResult ActionTurn::do_abort_work_(AnyMap& params)
{
  return Action::StateResult::Success();
}