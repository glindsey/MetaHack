#include "stdafx.h"

#include "ActionWait.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionWait ActionWait::prototype;
  ActionWait::ActionWait() : Action("wait", "WAIT", ActionWait::create_) {}
  ActionWait::ActionWait(EntityId subject) : Action(subject, "wait", "WAIT") {}
  ActionWait::~ActionWait() {}

  std::unordered_set<Trait> const & ActionWait::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly
    };

    return traits;
  }

  StateResult ActionWait::do_prebegin_work_(AnyMap& params)
  {
    // We can always wait.
    return StateResult::Success();
  }

  StateResult ActionWait::do_begin_work_(AnyMap& params)
  {
    /// @todo Handle a variable amount of time.
    std::string message = maketr("YOU_VERB_FOR_X_TIME", { std::to_string(1) });
    Service<IMessageLog>::get().add(message);

    return{ true, 1 };
  }

  StateResult ActionWait::do_finish_work_(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionWait::do_abort_work_(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

