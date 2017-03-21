#include "stdafx.h"

#include "ActionDie.h"
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

  std::unordered_set<Trait> const & ActionDie::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly,
      Trait::SubjectCanBeInLimbo
    };

    return traits;
  }

  StateResult ActionDie::do_prebegin_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::do_begin_work_(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;
    auto& subject = get_subject();

    /// @todo Handle stuff like auto-activating life-saving items here.
    /// @todo Pass in the cause of death somehow.
    if (subject->perform_intransitive_action(*this) == ActionResult::Success)
    {
      if (subject->is_player())
      {
        put_msg(maketr("YOU_DIE"));
      }
      else
      {
        bool living = subject->get_modified_property("living").as<bool>();
        put_msg(maketr(living ? "YOU_ARE_KILLED" : "YOU_ARE_DESTROYED"));
      }

      // Set the property saying the entity is dead.
      subject->set_base_property("dead", Property::from(true));

      // Clear any pending actions.
      subject->clear_pending_actions();

      /// @todo If Entity is the player, perform graceful shutdown of the game.
      return StateResult::Success();
    }
    else
    {
      put_msg(maketr("YOU_MANAGE_TO_AVOID_DYING"));
      return StateResult::Failure();
    }
  }

  StateResult ActionDie::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionDie::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
}

