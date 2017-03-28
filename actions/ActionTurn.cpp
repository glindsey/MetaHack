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

  StateResult ActionTurn::doPreBeginWorkNVI(AnyMap& params)
  {
    // All checks handled in Action class via traits.
    return StateResult::Success();
  }

  /// @todo Implement me.
  StateResult ActionTurn::doBeginWorkNVI(AnyMap& params)
  {
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = getSubject();
    EntityId location = subject->getLocation();
    MapTile* current_tile = subject->getMapTile();
    Direction new_direction = getTargetDirection();

    if ((new_direction != Direction::Up) &&
      (new_direction != Direction::Down))
    {
      /// @todo Change facing direction.
      result = StateResult::Success();
    } // end else if (other direction)

    return result;
  }

  StateResult ActionTurn::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionTurn::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}
