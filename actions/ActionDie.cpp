#include "stdafx.h"

#include "ActionDie.h"
#include "components/ComponentManager.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"
#include "Service.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "utilities/StringTransforms.h"

namespace Actions
{
  ActionDie ActionDie::prototype;
  ActionDie::ActionDie() : Action("die", "DIE", ActionDie::create_) {}
  ActionDie::ActionDie(EntityId subject) : Action(subject, "die", "DIE") {}
  ActionDie::~ActionDie() {}

  bool ActionDie::subjectIsCapable() const
  {
    return COMPONENTS.health.existsFor(getSubject());
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

  StateResult ActionDie::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::doBeginWorkNVI(AnyMap& params)
  {
    EntityId subject = getSubject();

    /// @todo Handle stuff like auto-activating life-saving items here.
    /// @todo Pass in the cause of death somehow.
    if (subject->do_(*this))
    {
      if (subject->isPlayer())
      {
        putTr("YOU_DIE");
      }
      else
      {
        bool living = COMPONENTS.health[subject].isLivingCreature();
        putMsg(makeTr(living ? "YOU_ARE_KILLED" : "YOU_ARE_DESTROYED"));
      }

      // Set the property saying the entity is dead.
      COMPONENTS.health[subject].setDead(true);

      // Clear any pending actions.
      subject->clearAllPendingActions();

      /// @todo If Entity is the player, perform graceful shutdown of the game.
      return StateResult::Success();
    }
    else
    {
      putTr("YOU_MANAGE_TO_AVOID_DYING");
      return StateResult::Failure();
    }
  }

  StateResult ActionDie::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}

