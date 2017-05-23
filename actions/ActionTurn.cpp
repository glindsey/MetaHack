#include "stdafx.h"

#include "ActionTurn.h"

#include "ActionAttack.h"
#include "components/ComponentManager.h"
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

  ReasonBool ActionTurn::subjectIsCapable(GameState const& gameState) const
  {
    auto subject = getSubject();
    bool isTurnable = (COMPONENTS.mobility.existsFor(subject) && COMPONENTS.mobility[subject].turnSpeed() > 0);
    std::string reason = isTurnable ? "" : "YOU_HAVE_NO_WAY_OF_TURNING"; ///< @todo Add translation key
    return { isTurnable, reason };      
  }

  std::unordered_set<Trait> const & ActionTurn::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbDirection,
      Trait::SubjectMustBeAbleToMove
    };

    return traits;
  }

  StateResult ActionTurn::doPreBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    // All checks handled in Action class via traits.
    return StateResult::Success();
  }

  /// @todo Implement me.
  StateResult ActionTurn::doBeginWorkNVI(GameState& gameState, SystemManager& systems)
  {
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = getSubject();
    auto position = COMPONENTS.position[subject];
    Direction new_direction = getTargetDirection();

    if ((new_direction != Direction::Up) &&
      (new_direction != Direction::Down))
    {
      /// @todo Change facing direction.
      result = StateResult::Success();
    } // end else if (other direction)

    return result;
  }

  StateResult ActionTurn::doFinishWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }

  StateResult ActionTurn::doAbortWorkNVI(GameState& gameState, SystemManager& systems)
  {
    return StateResult::Success();
  }
}
