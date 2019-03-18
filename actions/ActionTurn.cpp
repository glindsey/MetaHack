#include "ActionTurn.h"

#include "ActionAttack.h"
#include "components/ComponentManager.h"
#include "config/Strings.h"
#include "game/GameState.h"
#include "map/Map.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"

namespace Actions
{
  ActionTurn ActionTurn::prototype;
  ActionTurn::ActionTurn() : Action("TURN", ActionTurn::create_) {}
  ActionTurn::ActionTurn(EntityId subject) : Action(subject, "TURN") {}
  ActionTurn::~ActionTurn() {}

  ReasonBool ActionTurn::subjectIsCapable(GameState const& gameState) const
  {
    auto& components = gameState.components();
    auto subject = getSubject();
    bool isTurnable = (components.mobility.existsFor(subject) && components.mobility.of(subject).turnSpeed() > 0);
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

  StateResult ActionTurn::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    // All checks handled in Action class via traits.
    return StateResult::Success();
  }

  /// @todo Implement me.
  StateResult ActionTurn::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    auto& components = gameState.components();
    StateResult result = StateResult::Failure();

    std::string message;

    auto subject = getSubject();
    auto position = components.position.of(subject);
    Direction new_direction = getTargetDirection();

    if ((new_direction != Direction::Up) &&
      (new_direction != Direction::Down))
    {
      /// @todo Change facing direction.
      result = StateResult::Success();
    } // end else if (other direction)

    return result;
  }

  StateResult ActionTurn::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionTurn::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
}
