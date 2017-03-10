#include "stdafx.h"

#include "ActionWait.h"
#include "IMessageLog.h"
#include "IStringDictionary.h"
#include "Service.h"
#include "Entity.h"
#include "EntityId.h"

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
    /// @todo Temporary message, remove or change
    std::string message = make_string("$you successfully $(cv?stay:stays) where $you_subj $are.");

    //YOU + " successfully" + CV(" stay", " stays") +
    //" where " + YOU_SUBJ_ARE + ".";

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

