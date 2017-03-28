#include "stdafx.h"

#include "ActionClose.h"
#include "services/IMessageLog.h"
#include "services/IStringDictionary.h"
#include "Service.h"
#include "entity/Entity.h"
#include "entity/EntityId.h"

namespace Actions
{
  ActionClose ActionClose::prototype;
  ActionClose::ActionClose() : Action("close", "CLOSE", ActionClose::create_) {}
  ActionClose::ActionClose(EntityId subject) : Action(subject, "close", "CLOSE") {}
  ActionClose::~ActionClose() {}

  std::unordered_set<Trait> const & ActionClose::getTraits() const
  {
    static std::unordered_set<Trait> traits =
    {
      Trait::CanBeSubjectVerbObject,
      Trait::CanBeSubjectVerbDirection
    };

    return traits;
  }

  StateResult ActionClose::doPreBeginWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::doBeginWorkNVI(AnyMap& params)
  {
    bool success = false;
    unsigned int action_time = 0;

    putTr("ACTN_NOT_IMPLEMENTED");

#if 0
    if (entity != EntityId::Mu())
    {
      success = actor->do_close(entity, action_time);
    }
#endif

    return{ success, action_time };
  }

  StateResult ActionClose::doFinishWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }

  StateResult ActionClose::doAbortWorkNVI(AnyMap& params)
  {
    return StateResult::Success();
  }
}

