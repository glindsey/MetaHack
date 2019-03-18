#include "ActionDie.h"

#include "components/ComponentManager.h"
#include "config/Strings.h"
#include "lua/LuaObject.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"
#include "utilities/Shortcuts.h"
#include "utilities/StringTransforms.h"

namespace Actions
{
  ActionDie ActionDie::prototype;
  ActionDie::ActionDie() : Action("DIE", ActionDie::create_) {}
  ActionDie::ActionDie(EntityId subject) : Action(subject, "DIE") {}
  ActionDie::~ActionDie() {}

  ReasonBool ActionDie::subjectIsCapable(GameState const& gameState) const
  {
    bool hasHealth = COMPONENTS.health.existsFor(getSubject());
    std::string reason = hasHealth ? "" : "YOU_ARE_NOT_MORTAL"; ///< @todo Add translation key
    return { hasHealth, reason };
  }

  std::unordered_set<Trait> const & ActionDie::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly,
      Trait::SubjectCanBeInLimbo
    };

    return traits;
  }

  StateResult ActionDie::doPreBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::doBeginWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    EntityId subject = getSubject();
    auto& components = gameState.components();
    auto& lua = gameState.lua();
    auto& narrator = systems.narrator();

    /// @todo Handle stuff like auto-activating life-saving items here.
    /// @todo Pass in the cause of death somehow.
    if (lua.doReflexiveAction(subject, *this))
    {
      if (components.globals.player() == subject)
      {
        putMsg(narrator.makeTr("YOU_DIE", arguments));
      }
      else
      {
        bool living = components.health.of(subject).isLivingCreature();
        putMsg(narrator.makeTr(living ? "YOU_ARE_KILLED" : "YOU_ARE_DESTROYED", arguments));
      }

      // Set the property saying the entity is dead.
      COMPONENTS.health[subject].setDead(true);

      // Clear any pending actions.
      if (components.activity.existsFor(subject))
      {
        components.activity.of(subject).pendingActions().clear();
      }

      /// @todo If Entity is the player, perform graceful shutdown of the game.
      return StateResult::Success();
    }
    else
    {
      putMsg(narrator.makeTr("YOU_MANAGE_TO_AVOID_DYING", arguments));
      return StateResult::Failure();
    }
  }

  StateResult ActionDie::doFinishWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::doAbortWorkNVI(GameState& gameState, Systems::Manager& systems, json& arguments)
  {
    return StateResult::Success();
  }
}

