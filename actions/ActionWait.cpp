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

  bool ActionWait::subjectIsCapable() const
  {
    // Any entity can wait, whenever.
    return true;
  }

  std::unordered_set<Trait> const & ActionWait::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectOnly
    };

    return traits;
  }

  StateResult ActionWait::doPreBeginWorkNVI(AnyMap& params)
  {
    // We can always wait.
    return StateResult::Success();
  }

  StateResult ActionWait::doBeginWorkNVI(AnyMap& params)
  {
    /// @todo Handle a variable amount of time.
    putMsg(makeTr("YOU_VERB_FOR_X_TIME", 
                  { std::to_string(1) }));

    return{ true, 1 };
  }

  StateResult ActionWait::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionWait::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
} // end namespace

