#include "stdafx.h"

#include "ActionTurn.h"

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
  ActionTurn ActionTurn::prototype;
  ActionTurn::ActionTurn() : Action("turn", "TURN", ActionTurn::create_) {}
  ActionTurn::ActionTurn(EntityId subject) : Action(subject, "turn", "TURN") {}
  ActionTurn::~ActionTurn() {}

  std::unordered_set<Trait> const & ActionTurn::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbDirection,
      Trait::SubjectMustBeAbleToMove
    };

    return traits;
  }

  StateResult ActionTurn::do_prebegin_work_(AnyMap& params)
  {
    // All checks handled in Action class via traits.
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
